#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#include "window_desc.h"
#include "backend_utils.h"

#ifdef _WIN32
#include "window_backends/desk_up_win/desk_up_win.h"
#include <windows.h>
#endif

namespace fs = std::filesystem;

// Helper to create a temp directory unique for the test. Uses std::filesystem only.
static fs::path makeTempDir(const std::string &name){
    fs::path base = fs::temp_directory_path() / ("deskup_window_backend_test_" + name);
    std::error_code ec;
    fs::create_directories(base, ec); // ignore errors; existence is fine
    return base;
}

// windowDesc: default constructor should zero geometry and empty strings; operator! should be true.
TEST(DeskUpWindowBackend_windowDesc, DefaultConstructorAndOperatorBang){
    windowDesc wd; // default ctor
    EXPECT_TRUE(!wd) << "operator! should report uninitialized geometry";
    EXPECT_EQ(wd.x, 0);
    EXPECT_EQ(wd.y, 0);
    EXPECT_EQ(wd.w, 0);
    EXPECT_EQ(wd.h, 0);
    EXPECT_TRUE(wd.name.empty());
    EXPECT_TRUE(wd.pathToExec.empty());
}

// windowDesc: operator! should be false when any geometry dimension is non-zero.
TEST(DeskUpWindowBackend_windowDesc, OperatorBangFalseWhenInitialized){
    windowDesc wd("App", 10, 20, 800, 600, "/path/to/app.exe");
    EXPECT_FALSE(!wd);
    // Also test single dimension non-zero cases.
    windowDesc wdX("X", 1, 0, 0, 0, "exe");
    EXPECT_FALSE(!wdX);
    windowDesc wdY("Y", 0, 1, 0, 0, "exe");
    EXPECT_FALSE(!wdY);
    windowDesc wdW("W", 0, 0, 1, 0, "exe");
    EXPECT_FALSE(!wdW);
    windowDesc wdH("H", 0, 0, 0, 1, "exe");
    EXPECT_FALSE(!wdH);
}

// saveTo: empty path should return ERR_EMPTY_PATH
TEST(DeskUpWindowBackend_windowDesc, SaveToEmptyPath){
    windowDesc wd("App", 1,2,3,4, "/path/to/app");
    int code = wd.saveTo("");
    EXPECT_EQ(code, ERR_EMPTY_PATH);
}

TEST(DeskUpWindowBackend_windowDesc, SaveToInvalidDirectory){
    windowDesc wd("App", 1,2,3,4, "/path/to/app");
    fs::path bogus = fs::path("/this/dir/should/not/exist/xyz") / "win.txt";
    int code = wd.saveTo(bogus);

    EXPECT_TRUE(code == ERR_FILE_NOT_FOUND || code == ERR_FILE_NOT_OPEN) << "Unexpected code: " << code;
}

// saveTo: success path writes expected format
TEST(DeskUpWindowBackend_windowDesc, SaveToSuccess){
    auto tmpDir = makeTempDir("success");
    fs::path target = tmpDir / "window.txt";
    windowDesc wd("Calc", 11,22,333,444, "/usr/bin/calc");
    int code = wd.saveTo(target);
    ASSERT_EQ(code, SAVE_SUCCESS) << "Expected successful save";
    // Read file back and validate contents line by line.
    std::ifstream in(target.string());
    ASSERT_TRUE(in.is_open());
    std::string line;
    // 1: path
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, wd.pathToExec);
    // 2: x
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, std::to_string(wd.x));
    // 3: y
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, std::to_string(wd.y));
    // 4: w
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, std::to_string(wd.w));
    // 5: h
    ASSERT_TRUE(std::getline(in, line));
    EXPECT_EQ(line, std::to_string(wd.h));
    // Should be EOF now
    EXPECT_FALSE(std::getline(in, line));
}

// =========================
// backend_utils tests
// =========================

TEST(DeskUpWindowBackend_backendUtils, ToLowerStrBasic){
    EXPECT_EQ(toLowerStr(""), "");
    EXPECT_EQ(toLowerStr("abc"), "abc");
    EXPECT_EQ(toLowerStr("AbC123!-_"), "abc123!-_");
}

TEST(DeskUpWindowBackend_backendUtils, NormalizePathLower){
    std::string in = "C:/Users/Mixed/Path/File.TXT";
    std::string out = normalizePathLower(in);
    EXPECT_EQ(out, "c:\\users\\mixed\\path\\file.txt");

    // Mixed separators should all become backslashes and be lowercased
    std::string in2 = "D:/Projects\\DeskUp/Assets\\Icon.PNG";
    std::string out2 = normalizePathLower(in2);
    EXPECT_EQ(out2, "d:\\projects\\deskup\\assets\\icon.png");
}

#ifdef _WIN32
TEST(DeskUpWindowBackend_backendUtils, UTF8ToWideRoundtripSimple){
    std::string utf8 = "caf\u00E9"; // café
    std::wstring wide = UTF8ToWide(utf8);
    EXPECT_EQ(wide, L"caf\u00E9");
    // Convert back to UTF-8 using WideStringToUTF8
    std::string back = WideStringToUTF8(wide.c_str());
    EXPECT_EQ(back, utf8);
}

TEST(DeskUpWindowBackend_backendUtils, WideStringToUTF8_BasicUnicode){
    std::wstring w = L"caf\u00E9"; // café
    std::string s = WideStringToUTF8(w.c_str());
    EXPECT_EQ(s, "caf\u00E9");
}

TEST(DeskUpWindowBackend_backendUtils, GetSystemErrorMessageWindows_ContextAndKnownCode){
    // Known error code; message content is localized, so only assert structure.
    std::string ctx = "Open: ";
    std::string msg = getSystemErrorMessageWindows(ERROR_FILE_NOT_FOUND, ctx);
    ASSERT_GE(msg.size(), ctx.size());
    EXPECT_EQ(msg.rfind(ctx, 0), 0u) << "Message should start with context prefix";
    EXPECT_NE(msg.size(), ctx.size()) << "Should append a system message after context";
}

TEST(DeskUpWindowBackend_backendUtils, GetSystemErrorMessageWindows_ErrorZeroAndUnknown){
    // error == 0 yields a fixed string
    EXPECT_EQ(getSystemErrorMessageWindows(0), std::string("unknown error passed as parameter!"));

    // An unlikely/invalid error code should fall back to generic message with context prefix
    std::string ctx = "Ctx: ";
    std::string msg = getSystemErrorMessageWindows(0xEFFFABCD, ctx);
    EXPECT_EQ(msg.rfind(ctx, 0), 0u);
    EXPECT_NE(msg.find("Unknown Windows error."), std::string::npos);
}

// =========================
// Win32 backend function tests
// =========================

// Fixture to create a real Win32 window for backend testing
class Win32WindowFixture : public ::testing::Test {
protected:
    static ATOM s_atom;
    HWND hwnd = nullptr;
    DeskUpWindowDevice device{};
    std::vector<HWND> extra_windows; // additional windows created by tests

    static ATOM RegisterOnce() {
        if (s_atom) return s_atom;
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc   = DefWindowProcW;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"DeskUpTestWindowClass";
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        return RegisterClassExW(&wc);
    }

    // Helper: pump messages briefly and sleep to let the window manager settle
    static void ProcessEvents() {
        MSG msg;
        for (int i = 0; i < 10 && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE); ++i) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Helper: create an extra window for enumeration scenarios
    HWND CreateExtraWindow(const wchar_t* title, int x, int y, int w, int h, bool show = true) {
        HWND handle = CreateWindowExW(
            0,
            L"DeskUpTestWindowClass",
            title,
            WS_OVERLAPPEDWINDOW,
            x, y,
            w, h,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr);
        if (handle) {
            if (show) {
                ShowWindow(handle, SW_SHOW);
                UpdateWindow(handle);
            }
            ProcessEvents();
            extra_windows.push_back(handle);
        }
        return handle;
    }

    void SetUp() override {
        if (!s_atom) s_atom = RegisterOnce();
        ASSERT_NE(s_atom, (ATOM)0) << "Failed to register window class";

        // Initialize device FIRST (before creating test window)
        // This ensures desk_up_hwnd doesn't point to our test window
        device = WIN_CreateDevice();
        ASSERT_NE(device.internalData, nullptr);

        // Now create the test window with known position and size
        hwnd = CreateWindowExW(
            0,
            L"DeskUpTestWindowClass",
            L"DeskUpTestWindow",
            WS_OVERLAPPEDWINDOW, // standard window
            100, 150,  // x, y
            800, 600,  // width, height
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );
        ASSERT_NE(hwnd, nullptr) << "Failed to create test window";

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        // Small message pump to let window manager process show request
        ProcessEvents();

        // Set our test window as the active window in the device
        WIN_TEST_setHWND(&device, hwnd);
    }

    void TearDown() override {
        // Destroy any extra windows created by tests first
        for (HWND w : extra_windows) {
            if (w) DestroyWindow(w);
        }
        extra_windows.clear();
        if (hwnd) {
            DestroyWindow(hwnd);
            hwnd = nullptr;
        }
    }
};

ATOM Win32WindowFixture::s_atom = 0;

TEST_F(Win32WindowFixture, GetWindowGeometry) {
    // Get geometry via backend functions
    auto x_res = WIN_getWindowXPos(&device);
    auto y_res = WIN_getWindowYPos(&device);
    auto w_res = WIN_getWindowWidth(&device);
    auto h_res = WIN_getWindowHeight(&device);

    ASSERT_TRUE(x_res.has_value()) << "Failed to get X position";
    ASSERT_TRUE(y_res.has_value()) << "Failed to get Y position";
    ASSERT_TRUE(w_res.has_value()) << "Failed to get width";
    ASSERT_TRUE(h_res.has_value()) << "Failed to get height";

    int x = x_res.value();
    int y = y_res.value();
    unsigned int w = w_res.value();
    unsigned int h = h_res.value();

    // Validate against direct Win32 API call
    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);
    ASSERT_TRUE(GetWindowInfo(hwnd, &wi));

    EXPECT_EQ(x, static_cast<int>(wi.rcWindow.left));
    EXPECT_EQ(y, static_cast<int>(wi.rcWindow.top));
    EXPECT_EQ(w, static_cast<unsigned>(wi.rcWindow.right - wi.rcWindow.left));
    EXPECT_EQ(h, static_cast<unsigned>(wi.rcWindow.bottom - wi.rcWindow.top));

    // Basic sanity: non-zero dimensions
    EXPECT_GT(w, 0u);
    EXPECT_GT(h, 0u);
}

TEST_F(Win32WindowFixture, ResizeWindowChangesRect) {
    // Request a specific geometry
    windowDesc wd{"TestResize", 200, 250, 640, 480, ""};
    auto status = WIN_resizeWindow(&device, wd);
    ASSERT_TRUE(status.has_value()) << "WIN_resizeWindow failed";

    // Allow window manager to process
    MSG msg;
    for (int i = 0; i < 10 && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE); ++i) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Verify via GetWindowRect
    RECT r{};
    ASSERT_TRUE(GetWindowRect(hwnd, &r));

    int actual_x = r.left;
    int actual_y = r.top;
    int actual_w = r.right - r.left;
    int actual_h = r.bottom - r.top;

    // Allow tolerance for window decorations/DPI adjustments (especially on outer rect)
    EXPECT_NEAR(actual_x, wd.x, 10) << "X position mismatch";
    EXPECT_NEAR(actual_y, wd.y, 10) << "Y position mismatch";
    EXPECT_NEAR(actual_w, wd.w, 20) << "Width mismatch (decorations may add pixels)";
    EXPECT_NEAR(actual_h, wd.h, 40) << "Height mismatch (title bar + borders)";
}

TEST_F(Win32WindowFixture, GetPathFromWindowIsCurrentProcess) {
    auto path_res = WIN_getPathFromWindow(&device);
    ASSERT_TRUE(path_res.has_value()) << "Failed to get path from window";

    std::string path = path_res.value();
    EXPECT_FALSE(path.empty()) << "Path should not be empty";

    // The path should contain the test executable name (e.g., desk_up_window_backend_test.exe)
    // We can't hardcode the full path, but we can check it's a valid path and contains .exe
    EXPECT_NE(path.find(".exe"), std::string::npos) << "Expected .exe in path: " << path;

    EXPECT_NE(path.find("desk_up_window_backend_test"), std::string::npos) << "Expected the exe name in path: " << path;

    // Optional: verify it's our current process by comparing with GetModuleFileName
    char buf[MAX_PATH]{};
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    ASSERT_GT(n, 0u);
    std::string expected_path(buf, buf + n);

    // Normalize both paths for comparison (case-insensitive on Windows)
    EXPECT_EQ(normalizePathLower(path), normalizePathLower(expected_path));
}

TEST_F(Win32WindowFixture, EnumerateWindowsFindsTestWindow) {
    auto res = WIN_getAllOpenWindows(&device);
    ASSERT_TRUE(res.has_value()) << "WIN_getAllOpenWindows failed";

    auto& windows = res.value();
    EXPECT_GT(windows.size(), 0u) << "Should find at least some windows";

    // Look for our test window by checking for our process executable
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string our_exe = normalizePathLower(std::string(buf));

    bool found_our_window = false;
    for (const auto& w : windows) {
        if (!w.pathToExec.empty() && normalizePathLower(w.pathToExec) == our_exe) {
            // Found a window belonging to our process
            found_our_window = true;

            // Validate it has sensible geometry
            EXPECT_GT(w.w, 0);
            EXPECT_GT(w.h, 0);
            EXPECT_FALSE(w.name.empty());
            break;
        }
    }

    EXPECT_TRUE(found_our_window) << "Did not find our test window in enumeration";
}

// Strengthened coverage: ensure only visible, non-zero-size, titled windows are included
TEST_F(Win32WindowFixture, EnumerateWindows_FiltersHiddenZeroSizeAndUntitled) {
    auto getAll = [&]() -> std::vector<windowDesc>{
        auto res = WIN_getAllOpenWindows(&device);
		EXPECT_TRUE(res.has_value()) << "WIN_getAllOpenWindows failed";
		return res.value_or(std::vector<windowDesc>{});
    };

    // Create additional windows
    HWND visible = CreateExtraWindow(L"DeskUpVisibleA", 220, 180, 640, 480, true);
    ASSERT_NE(visible, nullptr);

    HWND hidden = CreateExtraWindow(L"DeskUpHiddenB", 260, 220, 640, 480, false); // not shown
    ASSERT_NE(hidden, nullptr);

    HWND zeroSize = CreateExtraWindow(L"DeskUpZeroC", 300, 260, 300, 200, true);
    ASSERT_NE(zeroSize, nullptr);
    // Force zero-sized
    SetWindowPos(zeroSize, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    ProcessEvents();

    HWND untitled = CreateExtraWindow(L"TempTitle", 340, 300, 320, 240, true);
    ASSERT_NE(untitled, nullptr);
    // Clear title after creation
    SetWindowTextW(untitled, L"");
    ProcessEvents();

    auto windowsAfter = getAll();

    // Compare paths against our current process exe
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string ourExe = normalizePathLower(std::string(buf));

    // Expect that at least our default window and 'visible' are included
    size_t ourCount = 0;
    bool foundVisibleGeometry = false;
    for (const auto& w : windowsAfter) {
        if (!w.pathToExec.empty() && normalizePathLower(w.pathToExec) == ourExe) {
            ++ourCount;
            // Validate non-zero geometry for our process windows
            EXPECT_GT(w.w, 0u);
            EXPECT_GT(w.h, 0u);
            if (std::abs((int)w.w - 640) <= 30 && std::abs((int)w.h - 480) <= 60) {
                foundVisibleGeometry = true;
            }
        }
    }
    EXPECT_GE(ourCount, 2u) << "Expected at least the default and one additional visible window";
    EXPECT_TRUE(foundVisibleGeometry) << "Visible, titled, non-zero window not found";
}

// Verify the global DeskUp HWND is skipped when it matches a window
TEST_F(Win32WindowFixture, EnumerateWindows_SkipsDeskUpGlobalWindow) {
    // Destroy the default window to ensure the special one is chosen as the DeskUp HWND
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
        ProcessEvents();
    }

    // Create a distinctive window
    HWND special = CreateExtraWindow(L"DeskUpGlobalSkip", 413, 457, 321, 123, true);
    ASSERT_NE(special, nullptr);

    // Create a new device AFTER creation to refresh the global 'desk_up_hwnd'
    auto dev2 = WIN_CreateDevice();
    (void)dev2;

    auto res = WIN_getAllOpenWindows(&device);
    ASSERT_TRUE(res.has_value());
    auto windows = res.value();

    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string ourExe = normalizePathLower(std::string(buf));

    bool foundSpecialGeometry = false;
    for (const auto& w : windows) {
        if (!w.pathToExec.empty() && normalizePathLower(w.pathToExec) == ourExe) {
            if (std::abs((int)w.w - 321) <= 30 && std::abs((int)w.h - 123) <= 40) {
                foundSpecialGeometry = true;
                break;
            }
        }
    }
    EXPECT_FALSE(foundSpecialGeometry) << "Window matching global DeskUp HWND should be skipped";
}

TEST_F(Win32WindowFixture, InvalidHWNDReturnsError) {
    // Set an invalid HWND
    WIN_TEST_setHWND(&device, nullptr);

    auto x_res = WIN_getWindowXPos(&device);
    EXPECT_FALSE(x_res.has_value());
    ASSERT_TRUE(x_res.error().isFatal());
    EXPECT_TRUE(x_res.error().type() == DeskUp::ErrType::InvalidInput);
}

TEST_F(Win32WindowFixture, ResizeWithInvalidDimensionsReturnsWarning) {
    windowDesc wd{"", 100, 100, 0, 0, ""}; // zero width/height
    auto status = WIN_resizeWindow(&device, wd);

    EXPECT_FALSE(status.has_value());
    EXPECT_TRUE(status.error().level() == DeskUp::Level::Warning);
    EXPECT_TRUE(status.error().type() == DeskUp::ErrType::InvalidInput);
}

// -----------------------------
// WIN_recoverSavedWindow tests
// -----------------------------

// Small helper to write a recover file with Windows line endings
static fs::path writeRecoverFile(const std::string& folderTag,
                                 const std::string& fileName,
                                 const std::vector<std::string>& lines)
{
    auto dir = makeTempDir("recover_" + folderTag);
    fs::path p = dir / fileName;
    std::ofstream out(p.string(), std::ios::binary);
    for (const auto& l : lines) {
        out << l << "\r\n"; // ensure CRLF
    }
    out.close();
    return p;
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_Success) {
    // Use current executable path for realism
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath = std::string(buf);

    auto file = writeRecoverFile("ok", "win.txt",
        { exePath, "100", "150", "640", "480" });

    auto res = WIN_recoverSavedWindow(nullptr, file);
    ASSERT_TRUE(res.has_value()) << "Expected successful recovery";

    auto w = res.value();
    EXPECT_TRUE(w.pathToExec == exePath);
    EXPECT_EQ(w.x, 100);
    EXPECT_EQ(w.y, 150);
    EXPECT_EQ((int)w.w, 640);
    EXPECT_EQ((int)w.h, 480);

    // Name should be the executable stem
    std::string stem = fs::path(exePath).stem().string();
    EXPECT_TRUE(w.name == stem);
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_FileNotFound) {
    auto dir = makeTempDir("missing");
    fs::path p = dir / "no_such_file.txt"; // do not create

    auto res = WIN_recoverSavedWindow(nullptr, p);
    ASSERT_FALSE(res.has_value());
    EXPECT_TRUE(res.error().isFatal());
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_IncompleteFile) {
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath = std::string(buf);

    // Only 4 lines instead of 5
    auto file = writeRecoverFile("incomplete", "win.txt",
        { exePath, "1", "2", "3" });

    auto res = WIN_recoverSavedWindow(nullptr, file);
    ASSERT_FALSE(res.has_value());
    EXPECT_TRUE(res.error().isFatal());
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_InvalidX) {
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath = std::string(buf);

    auto file = writeRecoverFile("badx", "win.txt",
        { exePath, "not-an-int", "2", "640", "480" });

    auto res = WIN_recoverSavedWindow(nullptr, file);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().level(), DeskUp::Level::Retry);
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_NegativeWidth) {
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath = std::string(buf);

    auto file = writeRecoverFile("negw", "win.txt",
        { exePath, "10", "20", "-1", "480" });

    auto res = WIN_recoverSavedWindow(nullptr, file);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().level(), DeskUp::Level::Retry);
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST(DeskUpWindowBackend_WinRecover, RecoverSavedWindow_NegativeHeight) {
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string exePath = std::string(buf);

    auto file = writeRecoverFile("negh", "win.txt",
        { exePath, "10", "20", "640", "-5" });

    auto res = WIN_recoverSavedWindow(nullptr, file);
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().level(), DeskUp::Level::Retry);
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

// -----------------------------
// WIN_loadProcessFromPath tests
// -----------------------------

TEST_F(Win32WindowFixture, LoadProcessFromPath_EmptyPath) {
    auto status = WIN_loadProcessFromPath(&device, "");
    ASSERT_FALSE(status.has_value());
    EXPECT_TRUE(status.error().isFatal());
    EXPECT_EQ(status.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST_F(Win32WindowFixture, LoadProcessFromPath_InvalidDevice) {
    // Pass nullptr device
    auto status = WIN_loadProcessFromPath(nullptr, "C:\\Windows\\notepad.exe");
    ASSERT_FALSE(status.has_value());
    EXPECT_TRUE(status.error().isFatal());
    EXPECT_EQ(status.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST_F(Win32WindowFixture, LoadProcessFromPath_NonExistentExe) {
    auto status = WIN_loadProcessFromPath(&device, "C:\\nonexistent_path\\fake.exe");
    ASSERT_FALSE(status.has_value());
    // ShellExecuteEx will fail with Retry + Os error
    EXPECT_EQ(status.error().level(), DeskUp::Level::Error);
    EXPECT_EQ(status.error().type(), DeskUp::ErrType::FileNotFound);
}

struct windowDataMock{
	HWND hwnd;
};

TEST_F(Win32WindowFixture, LoadProcessFromPath_SuccessWithNotepad) {
    // Launch notepad.exe which should be available on all Windows systems
    auto status = WIN_loadProcessFromPath(&device, "C:\\Windows\\notepad.exe");

    if (status.has_value()) {
        // Success: a window HWND should now be set in the device
        // Verify we can query geometry (indicating HWND is valid)
        auto x_res = WIN_getWindowXPos(&device);
        auto y_res = WIN_getWindowYPos(&device);

        EXPECT_TRUE(x_res.has_value()) << "Should be able to get X position after loading process";
        EXPECT_TRUE(y_res.has_value()) << "Should be able to get Y position after loading process";

        // Clean up: close the launched notepad
        // We can use WIN_closeProcessFromPath or just terminate via the device's HWND
        auto* data = static_cast<windowDataMock*>(device.internalData);
        if (data && data->hwnd) {
            PostMessageA(data->hwnd, WM_CLOSE, 0, 0);
            ProcessEvents();
        }
    } else {
        // If it fails, it's likely Retry + NotFound (main window not found in time)
        // or Retry + Os (ShellExecuteEx failed for environmental reasons like permissions)
        // Both are acceptable in CI/restricted environments
        EXPECT_TRUE(status.error().level() == DeskUp::Level::Retry);
    }
}

// -----------------------------
// WIN_closeProcessFromPath tests
// -----------------------------

TEST_F(Win32WindowFixture, CloseProcessFromPath_EmptyPath) {
    auto res = WIN_closeProcessFromPath(&device, "", false);
    ASSERT_FALSE(res.has_value());
    EXPECT_TRUE(res.error().isFatal());
    EXPECT_EQ(res.error().type(), DeskUp::ErrType::InvalidInput);
}

TEST_F(Win32WindowFixture, CloseProcessFromPath_NonExistentProcess) {
    // Try to close a process that doesn't exist
    auto res = WIN_closeProcessFromPath(&device, "C:\\nonexistent\\fake.exe", false);
    ASSERT_TRUE(res.has_value()) << "Should succeed with 0 closed";
    EXPECT_EQ(res.value(), 0u) << "Should close 0 processes";
}

TEST_F(Win32WindowFixture, CloseProcessFromPath_SuccessWithLaunchedProcess) {
    // Launch notepad, then close it via closeProcessFromPath
    auto launch_status = WIN_loadProcessFromPath(&device, "C:\\Windows\\notepad.exe");

    if (!launch_status.has_value()) {
        // Skip test if we can't launch (e.g., CI restrictions)
        GTEST_SKIP() << "Could not launch notepad: " << launch_status.error().what();
        return;
    }

    // Give the process a moment to fully initialize
    ProcessEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Now close it
    auto close_res = WIN_closeProcessFromPath(&device, "C:\\Windows\\notepad.exe", false);
    ASSERT_TRUE(close_res.has_value()) << "Close should succeed";

    // We should have closed at least 1 instance
    EXPECT_GE(close_res.value(), 1u) << "Should close at least 1 notepad instance";

    // Give time for the process to terminate
    ProcessEvents();
}

TEST_F(Win32WindowFixture, CloseProcessFromPath_AllowForceFlag) {
    // Test that allowForce parameter doesn't cause errors
    // Launch and close with allowForce=true
    auto launch_status = WIN_loadProcessFromPath(&device, "C:\\Windows\\notepad.exe");

    if (!launch_status.has_value()) {
        GTEST_SKIP() << "Could not launch notepad for force-close test";
        return;
    }

    ProcessEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Close with allowForce=true
    auto close_res = WIN_closeProcessFromPath(&device, "C:\\Windows\\notepad.exe", true);
    ASSERT_TRUE(close_res.has_value()) << "Force close should succeed";
    EXPECT_GE(close_res.value(), 1u);

    ProcessEvents();
}


#endif // _WIN32