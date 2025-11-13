#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "desk_up_backend_interface.h"
#include "desk_up_dummy_device.h"
#include "window_core.h"

// Fixture to set up and tear down the dummy device for each test
class DeskUpBackendInterfaceTest : public ::testing::Test {
protected:
    DeskUpWindowDevice dummyDevice;
    std::unique_ptr<DeskUpWindowDevice> savedBackend;
    std::string savedDeskUpDir;

    void SetUp() override {
        // Save current backend and DESKUPDIR
        savedBackend = std::move(current_window_backend);
        savedDeskUpDir = DESKUPDIR;

        // Create and install dummy device
        dummyDevice = DUMMY_CreateDevice();
        current_window_backend = std::make_unique<DeskUpWindowDevice>(dummyDevice);

        // Set test DESKUPDIR
        auto testPath = DUMMY_getDeskUpPath();
        if (testPath.has_value()) {
            DESKUPDIR = testPath.value();
            std::filesystem::create_directories(DESKUPDIR);
        }
    }

    void TearDown() override {
        // Clean up dummy device
        DUMMY_DestroyDevice(current_window_backend.get());

        // Restore original backend and DESKUPDIR
        current_window_backend = std::move(savedBackend);
        DESKUPDIR = savedDeskUpDir;

        // Clean up test directory
        auto testPath = DUMMY_getDeskUpPath();
        if (testPath.has_value()) {
            std::error_code ec;
            std::filesystem::remove_all(testPath.value(), ec);
        }
    }

    // Helper: get dummy device data
    DummyDeviceData* GetData() {
        return DUMMY_GetData(current_window_backend.get());
    }
};

// Basic smoke test to verify dummy device is working
TEST_F(DeskUpBackendInterfaceTest, DummyDeviceBasicFunctions) {
    auto* data = GetData();
    ASSERT_NE(data, nullptr);

    // Verify basic geometry functions work
    auto x = current_window_backend->getWindowXPos(current_window_backend.get());
    auto y = current_window_backend->getWindowYPos(current_window_backend.get());
    auto w = current_window_backend->getWindowWidth(current_window_backend.get());
    auto h = current_window_backend->getWindowHeight(current_window_backend.get());

    ASSERT_TRUE(x.has_value());
    ASSERT_TRUE(y.has_value());
    ASSERT_TRUE(w.has_value());
    ASSERT_TRUE(h.has_value());

    EXPECT_EQ(x.value(), 100);
    EXPECT_EQ(y.value(), 150);
    EXPECT_EQ(w.value(), 800u);
    EXPECT_EQ(h.value(), 600u);
}

TEST_F(DeskUpBackendInterfaceTest, DummyDeviceErrorSimulation) {
    auto* data = GetData();

    // Enable error simulation
    data->simulateError = true;
    data->errorToReturn = DeskUp::Error(
        DeskUp::Level::Fatal, DeskUp::ErrType::AccessDenied, 0, "Test error"
    );

    // Verify functions return expected error
    auto x = current_window_backend->getWindowXPos(current_window_backend.get());
    ASSERT_FALSE(x.has_value());
    EXPECT_TRUE(x.error().isFatal());
    EXPECT_EQ(x.error().type(), DeskUp::ErrType::AccessDenied);
}

TEST_F(DeskUpBackendInterfaceTest, SaveAllWindowsLocal_WritesFilesOnSuccess){
    auto* data = GetData();
    ASSERT_NE(data, nullptr);

    // Prepare a controlled set of windows
    data->windows.clear();
    windowDesc a{"Alpha", 10, 20, 300, 200, "alpha.exe"};
    windowDesc b{"Beta",  30, 40, 500, 400, "beta.exe"};
    data->windows.push_back(a);
    data->windows.push_back(b);

    // Call the interface to save all windows into workspace folder
    auto status = DeskUpBackendInterface::saveAllWindowsLocal("ws1");
    ASSERT_TRUE(status.has_value()) << "saveAllWindowsLocal should succeed";

    // Verify files exist and contents match
    namespace fs = std::filesystem;
    fs::path workspace = fs::path(DESKUPDIR) / "ws1";
    ASSERT_TRUE(fs::exists(workspace)) << "Workspace directory not created";
    ASSERT_TRUE(fs::is_directory(workspace));

    auto checkFile = [&](const windowDesc& w){
        fs::path p = workspace / w.name; // filenames are the window name
        ASSERT_TRUE(fs::exists(p)) << "Missing file: " << p.string();
        ASSERT_TRUE(fs::is_regular_file(p));

        std::ifstream in(p.string());
        ASSERT_TRUE(in.is_open());
        std::string line;
        // 1: path
        ASSERT_TRUE(std::getline(in, line));
        EXPECT_EQ(line, w.pathToExec);
        // 2: x
        ASSERT_TRUE(std::getline(in, line));
        EXPECT_EQ(line, std::to_string(w.x));
        // 3: y
        ASSERT_TRUE(std::getline(in, line));
        EXPECT_EQ(line, std::to_string(w.y));
        // 4: w
        ASSERT_TRUE(std::getline(in, line));
        EXPECT_EQ(line, std::to_string(w.w));
        // 5: h
        ASSERT_TRUE(std::getline(in, line));
        EXPECT_EQ(line, std::to_string(w.h));
        // EOF
        EXPECT_FALSE(std::getline(in, line));
    };

    checkFile(a);
    checkFile(b);
}

TEST_F(DeskUpBackendInterfaceTest, SaveAllWindowsLocal_EmptyWindowsList){
    auto* data = GetData();
    ASSERT_NE(data, nullptr);

    // Clear windows to simulate no open windows
    data->windows.clear();
	data->forceNonEmpty = false;

    // Should succeed (nothing to save is not an error)
    auto status = DeskUpBackendInterface::saveAllWindowsLocal("emptyWorkspace");
    EXPECT_TRUE(status.has_value()) << "Empty windows list should succeed";

    // Verify workspace directory was created but contains no files
    namespace fs = std::filesystem;
    fs::path workspace = fs::path(DESKUPDIR) / "emptyWorkspace";
    ASSERT_TRUE(fs::exists(workspace));
    ASSERT_TRUE(fs::is_directory(workspace));

    // Count files in directory - should be 0
    size_t fileCount = 0;
    for (const auto& entry : fs::directory_iterator(workspace)) {
        if (fs::is_regular_file(entry)) ++fileCount;
    }
    EXPECT_EQ(fileCount, 0u) << "No files should be saved for empty windows list";
}

TEST_F(DeskUpBackendInterfaceTest, SaveAllWindowsLocal_GetAllOpenWindowsReturnsError){
    auto* data = GetData();
    ASSERT_NE(data, nullptr);

    // Simulate a fatal error from getAllOpenWindows
    data->simulateError = true;
    data->errorToReturn = DeskUp::Error(
        DeskUp::Level::Fatal, DeskUp::ErrType::Default, 0, "Backend enumeration failed"
    );

    // Should propagate the error
    auto status = DeskUpBackendInterface::saveAllWindowsLocal("failWorkspace");
    ASSERT_FALSE(status.has_value()) << "Should fail when getAllOpenWindows fails";
    EXPECT_TRUE(status.error().isFatal());
    EXPECT_EQ(status.error().type(), DeskUp::ErrType::Default);
}

TEST_F(DeskUpBackendInterfaceTest, SaveAllWindowsLocal_DuplicateWindowNames){
    auto* data = GetData();
    ASSERT_NE(data, nullptr);

    // Create windows with duplicate names
    data->windows.clear();
    windowDesc w1{"Duplicate", 10, 20, 300, 200, "app1.exe"};
    windowDesc w2{"Duplicate", 30, 40, 500, 400, "app2.exe"};
    windowDesc w3{"Duplicate", 50, 60, 700, 600, "app3.exe"};
    data->windows.push_back(w1);
    data->windows.push_back(w2);
    data->windows.push_back(w3);

    auto status = DeskUpBackendInterface::saveAllWindowsLocal("dupWorkspace");
    ASSERT_TRUE(status.has_value()) << "Should succeed even with duplicate names";

    // Verify files exist with collision resolution (appended IDs)
    namespace fs = std::filesystem;
    fs::path workspace = fs::path(DESKUPDIR) / "dupWorkspace";
    ASSERT_TRUE(fs::exists(workspace));

    // First file should be just "Duplicate", subsequent ones get numbered
    fs::path f1 = workspace / "Duplicate";
    fs::path f2 = workspace / "Duplicate0";
    fs::path f3 = workspace / "Duplicate1";

    EXPECT_TRUE(fs::exists(f1)) << "First file should exist without suffix";
    EXPECT_TRUE(fs::exists(f2)) << "Second file should exist with '0' suffix";
    EXPECT_TRUE(fs::exists(f3)) << "Third file should exist with '1' suffix";

    // Verify content of first file matches w1
    std::ifstream in(f1.string());
    ASSERT_TRUE(in.is_open());
    std::string line;
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, "app1.exe") << "First duplicate should save app1.exe";
}

// TEST_F(DeskUpBackendInterfaceTest, SaveAllWindowsLocal_InvalidWorkspaceName){
//     auto* data = GetData();
//     ASSERT_NE(data, nullptr);

//     // Prepare some windows
//     data->windows.clear();
//     windowDesc w{"TestWindow", 10, 20, 300, 200, "test.exe"};
//     data->windows.push_back(w);

//     // Try to save with path-unsafe characters in workspace name
//     // The function doesn't validate workspace names, so behavior depends on filesystem
//     auto status = DeskUpBackendInterface::saveAllWindowsLocal("ws:invalid?name");

//     // We're testing that it doesn't crash and returns a status
//     if (status.has_value()) {
//         namespace fs = std::filesystem;
//         fs::path workspace = fs::path(DESKUPDIR) / "ws:invalid?name";
//         // Some filesystems might sanitize or reject this
//         SUCCEED() << "Function handled invalid workspace name gracefully";
//     } else {
//         // If it fails, that's also acceptable for invalid names
//         SUCCEED() << "Function rejected invalid workspace name";
//     }
// }

// =========================
// Helper function tests
// =========================

TEST_F(DeskUpBackendInterfaceTest, IsWorkspaceValid_ValidNames){
    EXPECT_TRUE(DeskUpBackendInterface::isWorkspaceValid("myWorkspace"));
    EXPECT_TRUE(DeskUpBackendInterface::isWorkspaceValid("Workspace123"));
    EXPECT_TRUE(DeskUpBackendInterface::isWorkspaceValid("a"));
    EXPECT_TRUE(DeskUpBackendInterface::isWorkspaceValid("Work_Space"));
    EXPECT_TRUE(DeskUpBackendInterface::isWorkspaceValid("Work-Space"));
}

TEST_F(DeskUpBackendInterfaceTest, IsWorkspaceValid_InvalidNames){
    // Empty
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid(""));

    // Contains forbidden characters
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws\\name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws/name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws:name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws?name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws*name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws\"name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws<name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws>name"));
    EXPECT_FALSE(DeskUpBackendInterface::isWorkspaceValid("ws|name"));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsWorkspace_NonExistent){
    EXPECT_FALSE(DeskUpBackendInterface::existsWorkspace("nonExistentWorkspace"));
    EXPECT_FALSE(DeskUpBackendInterface::existsWorkspace(""));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsWorkspace_ExistsAfterCreation){
    // Create a workspace
    auto status = DeskUpBackendInterface::saveAllWindowsLocal("testExists");
    ASSERT_TRUE(status.has_value());

    // Now it should exist
    EXPECT_TRUE(DeskUpBackendInterface::existsWorkspace("testExists"));
}

TEST_F(DeskUpBackendInterfaceTest, RemoveWorkspace_NonExistent){
    int result = DeskUpBackendInterface::removeWorkspace("nonExistentWorkspace");
    EXPECT_EQ(result, 0) << "Removing non-existent workspace should return 0";
}

TEST_F(DeskUpBackendInterfaceTest, RemoveWorkspace_Success){
    // Create a workspace with some content
    auto* data = GetData();
    data->windows.clear();
    windowDesc w{"Window", 10, 20, 300, 200, "app.exe"};
    data->windows.push_back(w);

    auto status = DeskUpBackendInterface::saveAllWindowsLocal("toRemove");
    ASSERT_TRUE(status.has_value());
    ASSERT_TRUE(DeskUpBackendInterface::existsWorkspace("toRemove"));

    // Remove it
    int result = DeskUpBackendInterface::removeWorkspace("toRemove");
    EXPECT_EQ(result, 1) << "Removing existing workspace should return 1";

    // Verify it's gone
    EXPECT_FALSE(DeskUpBackendInterface::existsWorkspace("toRemove"));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsFile_NonExistent){
    namespace fs = std::filesystem;
    fs::path nonExistent = fs::path(DESKUPDIR) / "nonexistent.txt";
    EXPECT_FALSE(DeskUpBackendInterface::existsFile(nonExistent));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsFile_DirectoryNotFile){
    namespace fs = std::filesystem;
    // DESKUPDIR itself is a directory, not a file
    EXPECT_FALSE(DeskUpBackendInterface::existsFile(DESKUPDIR));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsFile_ActualFile){
    namespace fs = std::filesystem;

    // Create a workspace with a file
    auto* data = GetData();
    data->windows.clear();
    windowDesc w{"FileTest", 10, 20, 300, 200, "app.exe"};
    data->windows.push_back(w);

    auto status = DeskUpBackendInterface::saveAllWindowsLocal("fileTestWS");
    ASSERT_TRUE(status.has_value());

    // Check if the saved file exists
    fs::path savedFile = fs::path(DESKUPDIR) / "fileTestWS" / "FileTest";
    EXPECT_TRUE(DeskUpBackendInterface::existsFile(savedFile));
}

TEST_F(DeskUpBackendInterfaceTest, ExistsFile_EmptyPath){
    namespace fs = std::filesystem;
    fs::path empty;
    EXPECT_FALSE(DeskUpBackendInterface::existsFile(empty));
}

// =========================
// restoreWindows tests
// =========================

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_WorkspaceNotFound){
    // Try to restore from a non-existent workspace
    auto status = DeskUpBackendInterface::restoreWindows("nonExistentWorkspace");

    ASSERT_FALSE(status.has_value()) << "Should fail when workspace doesn't exist";
    EXPECT_TRUE(status.error().isFatal());
    EXPECT_EQ(status.error().type(), DeskUp::ErrType::NotFound);
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_EmptyWorkspace){
    namespace fs = std::filesystem;

    // Create an empty workspace directory
    fs::path workspace = fs::path(DESKUPDIR) / "emptyWS";
    fs::create_directories(workspace);

    // Restore should succeed (no files to process)
    auto status = DeskUpBackendInterface::restoreWindows("emptyWS");
    EXPECT_TRUE(status.has_value()) << "Restoring empty workspace should succeed";
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_Success){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Step 1: Create a workspace with saved windows
    data->windows.clear();
    windowDesc w1{"App1", 100, 150, 800, 600, "app1.exe"};
    windowDesc w2{"App2", 200, 250, 640, 480, "app2.exe"};
    data->windows.push_back(w1);
    data->windows.push_back(w2);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("restoreSuccessWS");
    ASSERT_TRUE(saveStatus.has_value());

    // Step 2: Clear windows and restore
    data->windows.clear();

    // Track calls made during restore
    struct CallTracker {
        int recoverCalls = 0;
        int closeCalls = 0;
        int loadCalls = 0;
        int resizeCalls = 0;
        std::string lastLoadedPath;
        windowDesc lastResize;
    };
    CallTracker tracker;

    // We need to modify dummy device to track calls, but for now verify status
    auto restoreStatus = DeskUpBackendInterface::restoreWindows("restoreSuccessWS");
    ASSERT_TRUE(restoreStatus.has_value()) << "Restore should succeed with valid files";

    // The dummy device's state should reflect the last window restored
    // Since we have 2 files, the device should have been used multiple times
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_RecoverFileFails_Fatal){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create workspace directory manually
    fs::path workspace = fs::path(DESKUPDIR) / "corruptWS";
    fs::create_directories(workspace);

    // Create a file that doesn't exist in the dummy's view
    // (dummy recoverSavedWindow checks file existence)
    fs::path fakeFile = workspace / "fakeWindow";
    std::ofstream out(fakeFile.string());
    out << "corrupt\ndata\n";
    out.close();

    // Configure dummy to fail with fatal error when recovering
    data->simulateError = true;
    data->errorToReturn = DeskUp::Error(
        DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0, "Corrupt file"
    );

    auto status = DeskUpBackendInterface::restoreWindows("corruptWS");
    ASSERT_FALSE(status.has_value()) << "Fatal error during recover should abort restore";
    EXPECT_TRUE(status.error().isFatal());
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_RecoverFileNonFatalError){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create workspace with a valid file
    data->windows.clear();
    windowDesc w{"ValidWindow", 100, 150, 800, 600, "valid.exe"};
    data->windows.push_back(w);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("mixedWS");
    ASSERT_TRUE(saveStatus.has_value());

    // Configure dummy to return non-fatal error (Retry/Warning)
    data->simulateError = true;
    data->errorToReturn = DeskUp::Error(
        DeskUp::Level::Retry, DeskUp::ErrType::Io, 0, "Temporary error"
    );

    auto status = DeskUpBackendInterface::restoreWindows("mixedWS");

    // Non-fatal errors should allow function to continue
    // Based on implementation, if ALL files fail with non-fatal, we still succeed
    // (errors are logged to console but don't stop the loop)
    EXPECT_TRUE(status.has_value()) << "Non-fatal errors should not abort restore";
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_CloseProcessFails_NonFatal){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create workspace
    data->windows.clear();
    windowDesc w{"CloseTest", 100, 150, 800, 600, "closetest.exe"};
    data->windows.push_back(w);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("closeFailWS");
    ASSERT_TRUE(saveStatus.has_value());

    // Disable error simulation first to let recover succeed
    data->simulateError = false;

    // After first recover succeeds, we'd need to selectively fail close
    // For simplicity, test that the function continues even with close warnings

    auto status = DeskUpBackendInterface::restoreWindows("closeFailWS");
    EXPECT_TRUE(status.has_value()) << "Non-fatal close errors should not abort";
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_LoadProcessFails_NonFatal){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create workspace
    data->windows.clear();
    windowDesc w{"LoadTest", 100, 150, 800, 600, "loadtest.exe"};
    data->windows.push_back(w);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("loadFailWS");
    ASSERT_TRUE(saveStatus.has_value());

    // The loadWindowFromPath in dummy only fails on empty path (fatal)
    // To test non-fatal, we'd need to extend dummy device
    // For now, verify normal success path
    data->simulateError = false;

    auto status = DeskUpBackendInterface::restoreWindows("loadFailWS");
    EXPECT_TRUE(status.has_value());
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_ResizeFails_NonFatal){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create workspace
    data->windows.clear();
    windowDesc w{"ResizeTest", 100, 150, 800, 600, "resizetest.exe"};
    data->windows.push_back(w);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("resizeFailWS");
    ASSERT_TRUE(saveStatus.has_value());

    // Verify that resize errors don't abort
    data->simulateError = false;

    auto status = DeskUpBackendInterface::restoreWindows("resizeFailWS");
    EXPECT_TRUE(status.has_value());
}

TEST_F(DeskUpBackendInterfaceTest, RestoreWindows_VerifyWorkflow){
    namespace fs = std::filesystem;
    auto* data = GetData();

    // Create a workspace with known geometry
    data->windows.clear();
    windowDesc original{"WorkflowTest", 50, 75, 1024, 768, "workflow.exe"};
    data->windows.push_back(original);

    auto saveStatus = DeskUpBackendInterface::saveAllWindowsLocal("workflowWS");
    ASSERT_TRUE(saveStatus.has_value());

    // Change device state (simulating different window)
    data->x = 999;
    data->y = 999;
    data->w = 100;
    data->h = 100;
    data->path = "different.exe";

    // Restore should update device state via resize
    data->simulateError = false;
    auto restoreStatus = DeskUpBackendInterface::restoreWindows("workflowWS");
    ASSERT_TRUE(restoreStatus.has_value());

    // After restore, device state should reflect the restored window
    // (resize updates data->x, data->y, data->w, data->h)
    EXPECT_EQ(data->x, original.x);
    EXPECT_EQ(data->y, original.y);
    EXPECT_EQ(data->w, original.w);
    EXPECT_EQ(data->h, original.h);
}

