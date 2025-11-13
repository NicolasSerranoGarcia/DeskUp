#include <gtest/gtest.h>

#include <vector>

#include "desk_up_error.h"
#include "window_desc.h"

using namespace DeskUp;

std::vector<Level> l = {Level::Debug, Level::Default, Level::Error, Level::Fatal, Level::Info, Level::None, Level::Retry, Level::Warning};

TEST(DeskUpErrorTest, checkLevel){
    for(unsigned int i = 0; i < l.size(); i++){
        Error err(l[i], ErrType::None, 0, "");
        EXPECT_EQ(err.level(), l[i]);
    }
}

#ifdef _WIN32
// Test the overload that pulls GetLastError() directly.
TEST(DeskUpErrorTest, fromLastWinErrorGetLastError){
    struct Case { DWORD code; Level level; ErrType type; } cases[] = {
        { ERROR_ACCESS_DENIED,       Level::Fatal,  ErrType::AccessDenied },
        { ERROR_NOT_ENOUGH_MEMORY,   Level::Fatal,  ErrType::InsufficientMemory },
        { ERROR_SHARING_VIOLATION,   Level::Retry,  ErrType::SharingViolation },
        { ERROR_INVALID_PARAMETER,   Level::Fatal,  ErrType::InvalidInput },
        { ERROR_FILE_NOT_FOUND,      Level::Default,ErrType::InvalidInput },
        { ERROR_DISK_FULL,           Level::Fatal,  ErrType::Io },
        { ERROR_FUNCTION_FAILED,     Level::Retry,  ErrType::Unexpected },
        { 424242,                    Level::Default,ErrType::Default }
    };

    for(const auto &c : cases){
        ::SetLastError(c.code);
        Error e = Error::fromLastWinError("context", 1); // uses GetLastError()
        EXPECT_EQ(e.level(), c.level) << "GetLastError Code: " << c.code;
        EXPECT_EQ(e.type(), c.type) << "GetLastError Code: " << c.code;
    }
}
#endif
std::vector<ErrType> r = {
        ErrType::InsufficientMemory,
        ErrType::AccessDenied,
        ErrType::SharingViolation,
        ErrType::Io,
        ErrType::NotFound,
        ErrType::DiskFull,
        ErrType::DeviceNotFound,
        ErrType::Timeout,
        ErrType::ResourceBusy,
        ErrType::FileNotFound,
        ErrType::InvalidFormat,
        ErrType::InvalidInput,
        ErrType::CorruptedData,
        ErrType::OutOfRange,
        ErrType::NetworkError,
        ErrType::ConnectionRefused,
        ErrType::HostUnreachable,
        ErrType::ProtocolError,
        ErrType::Unexpected,
        ErrType::NotImplemented,
        ErrType::Default,
        ErrType::None
    };

TEST(DeskUpErrorTest, checkErrType){
    for(unsigned int i = 0; i < r.size(); i++){
        Error err(Level::None, r[i], 0, "");
        EXPECT_EQ(err.type(), r[i]);
    }
}

TEST(DeskUpErrorTest, checkAttempts){
    Error err(Level::None, ErrType::None, 3, "");
    EXPECT_EQ(err.attempts(), 3);
}

// whichWindow() should return a default windowDesc for a default constructed Error.
TEST(DeskUpErrorTest, whichWindowDefault){
    Error err; // default => Level::None
    windowDesc wd = err.whichWindow();
    // With windowDesc default constructor, geometry must be zero and strings empty.
    EXPECT_TRUE(!wd); // operator! checks x,y,w,h are all zero
    EXPECT_EQ(wd.x, 0);
    EXPECT_EQ(wd.y, 0);
    EXPECT_EQ(wd.w, 0);
    EXPECT_EQ(wd.h, 0);
    EXPECT_TRUE(wd.name.empty());
    EXPECT_TRUE(wd.pathToExec.empty());
}

// isFatal() and isRetriable() positive and negative cases.
TEST(DeskUpErrorTest, isFatalAndRetriable){
    Error fatalErr(Level::Fatal, ErrType::Unexpected, 0, "fatal");
    EXPECT_TRUE(fatalErr.isFatal());
    EXPECT_FALSE(fatalErr.isRetriable());

    Error retryErr(Level::Retry, ErrType::Io, 2, "retry");
    EXPECT_TRUE(retryErr.isRetriable());
    EXPECT_FALSE(retryErr.isFatal());

    Error normalErr(Level::Error, ErrType::Default, 0, "error");
    EXPECT_FALSE(normalErr.isFatal());
    EXPECT_FALSE(normalErr.isRetriable());
}

// bool operator should be false only for Level::None.
TEST(DeskUpErrorTest, booleanOperator){
    Error none; // Level::None
    EXPECT_FALSE(static_cast<bool>(none));
    Error info(Level::Info, ErrType::None, 0, "info");
    EXPECT_TRUE(static_cast<bool>(info));
    Error warn(Level::Warning, ErrType::Unexpected, 1, "warn");
    EXPECT_TRUE(static_cast<bool>(warn));
}

// Ensure operator bool consistency across all Level values.
TEST(DeskUpErrorTest, booleanOperatorAllLevels){
    std::vector<Level> levels = {Level::Fatal, Level::Error, Level::Warning, Level::Retry, Level::Info, Level::Debug, Level::Default, Level::None};
    for(auto lvl : levels){
        Error e(lvl, ErrType::Default, 0, "msg");
        bool expected = (lvl != Level::None);
        EXPECT_EQ(static_cast<bool>(e), expected) << "Level mismatch";
    }
}

// Message propagation test.
TEST(DeskUpErrorTest, messagePropagation){
    const std::string text = "custom error message";
    Error e(Level::Error, ErrType::Unexpected, 5, text);
    EXPECT_STREQ(e.what(), text.c_str());
}

// Attempts propagation: explicit tries for Windows error overload and negative code in fromSaveError.
#ifdef _WIN32
TEST(DeskUpErrorTest, attemptsPropagationWindows){
    ::SetLastError(ERROR_ACCESS_DENIED);
    Error e = Error::fromLastWinError("ctx", 7);
    EXPECT_EQ(e.attempts(), 7);
    ::SetLastError(ERROR_ACCESS_DENIED);
    Error e2 = Error::fromLastWinError("ctx");
    EXPECT_EQ(e2.attempts(), 0);
}
#endif

TEST(DeskUpErrorTest, attemptsPropagationSaveError){
    Error e = Error::fromSaveError(-3);
    EXPECT_EQ(e.attempts(), -3); // stores original code per implementation
}

// Copy and move stability tests.
TEST(DeskUpErrorTest, copyAndMoveStability){
    Error original(Level::Fatal, ErrType::AccessDenied, 9, "fatal access denied");
    Error copy = original; // copy construct
    EXPECT_EQ(copy.level(), original.level());
    EXPECT_EQ(copy.type(), original.type());
    EXPECT_EQ(copy.attempts(), original.attempts());
    EXPECT_STREQ(copy.what(), original.what());

    Error moved = std::move(original); // move construct
    EXPECT_EQ(moved.level(), Level::Fatal);
    EXPECT_EQ(moved.type(), ErrType::AccessDenied);
    EXPECT_EQ(moved.attempts(), 9);
    EXPECT_STREQ(moved.what(), "fatal access denied");
}


// fromSaveError mapping tests.
TEST(DeskUpErrorTest, fromSaveErrorMapping){
    // Success code 1
    {
        Error e = Error::fromSaveError(1);
        EXPECT_EQ(e.level(), Level::Info);
        EXPECT_EQ(e.type(), ErrType::None);
        EXPECT_EQ(e.attempts(), 1); // stores code
    }
    // ERR_EMPTY_PATH -1
    {
        Error e = Error::fromSaveError(-1);
        EXPECT_EQ(e.level(), Level::Error);
        EXPECT_EQ(e.type(), ErrType::InvalidInput);
    }
    // ERR_FILE_NOT_OPEN -2
    {
        Error e = Error::fromSaveError(-2);
        EXPECT_EQ(e.level(), Level::Error);
        EXPECT_EQ(e.type(), ErrType::Io);
    }
    // ERR_NO_PERMISSION -3
    {
        Error e = Error::fromSaveError(-3);
        EXPECT_EQ(e.level(), Level::Error);
        EXPECT_EQ(e.type(), ErrType::AccessDenied);
    }
    // ERR_FILE_NOT_FOUND -4
    {
        Error e = Error::fromSaveError(-4);
        EXPECT_EQ(e.level(), Level::Error);
        EXPECT_EQ(e.type(), ErrType::FileNotFound);
    }
    // ERR_DISK_FULL -5
    {
        Error e = Error::fromSaveError(-5);
        EXPECT_EQ(e.level(), Level::Fatal);
        EXPECT_EQ(e.type(), ErrType::DiskFull);
    }
    // ERR_UNKNOWN -6
    {
        Error e = Error::fromSaveError(-6);
        EXPECT_EQ(e.level(), Level::Error);
        EXPECT_EQ(e.type(), ErrType::Unexpected);
    }
    // Unrecognized code e.g. 42
    {
        Error e = Error::fromSaveError(42);
        EXPECT_EQ(e.level(), Level::Warning);
        EXPECT_EQ(e.type(), ErrType::Default);
    }
}

#include <filesystem>
namespace {
    std::filesystem::path tempTestDir(){
        auto p = std::filesystem::temp_directory_path() / "deskup_error_test";
        std::error_code ec; std::filesystem::create_directories(p, ec);
        return p;
    }
}

// Integration tests using windowDesc::saveTo and Error::fromSaveError
TEST(DeskUpErrorTest, fromSaveErrorIntegration){
    windowDesc wd; // default values; geometry zero; path empty
    // Empty path -> ERR_EMPTY_PATH
    int code = wd.saveTo("");
    Error e = Error::fromSaveError(code);
    EXPECT_EQ(code, -1); // ERR_EMPTY_PATH
    EXPECT_EQ(e.type(), ErrType::InvalidInput);

    // Non-existent directory -> create unique path inside a non-existing parent
    auto base = tempTestDir();
    auto nonExistParent = base / "nonexistent_parent" / "file.txt";
    code = wd.saveTo(nonExistParent);
    e = Error::fromSaveError(code);
    // Depending on implementation this could be FILE_NOT_FOUND or FILE_NOT_OPEN; we accept either mapping.
    EXPECT_TRUE(code == -4 || code == -2);
    if(code == -4) {
        EXPECT_EQ(e.type(), ErrType::FileNotFound);
    } else if(code == -2) {
        EXPECT_EQ(e.type(), ErrType::Io);
    }

    // Successful save: provide minimal data and a valid file in temp dir
    wd.pathToExec = "dummy.exe"; wd.x = 10; wd.y = 20; wd.w = 100; wd.h = 200;
    auto goodFile = base / "good_save.txt";
    code = wd.saveTo(goodFile);
    e = Error::fromSaveError(code);
    EXPECT_EQ(code, 1);
    EXPECT_EQ(e.level(), Level::Info);
    EXPECT_EQ(e.type(), ErrType::None);
    // Clean up
    std::error_code ec; std::filesystem::remove_all(base, ec);
}

#ifdef _WIN32
#include <Windows.h>
// fromLastWinError explicit code mapping
TEST(DeskUpErrorTest, fromLastWinErrorExplicit){
    struct Case { DWORD code; Level level; ErrType type; } cases[] = {
        { ERROR_ACCESS_DENIED,       Level::Fatal,  ErrType::AccessDenied },
        { ERROR_PRIVILEGE_NOT_HELD,  Level::Fatal,  ErrType::AccessDenied },
        { ERROR_NOT_ENOUGH_MEMORY,   Level::Fatal,  ErrType::InsufficientMemory },
        { ERROR_OUTOFMEMORY,         Level::Fatal,  ErrType::InsufficientMemory },
        { ERROR_SHARING_VIOLATION,   Level::Retry,  ErrType::SharingViolation },
        { ERROR_LOCK_VIOLATION,      Level::Retry,  ErrType::SharingViolation },
        { ERROR_INVALID_PARAMETER,   Level::Fatal,  ErrType::InvalidInput },
        { ERROR_INVALID_NAME,        Level::Fatal,  ErrType::InvalidInput },
        { ERROR_FILENAME_EXCED_RANGE,Level::Fatal,  ErrType::InvalidInput },
        { ERROR_FILE_NOT_FOUND,      Level::Default,ErrType::InvalidInput },
        { ERROR_PATH_NOT_FOUND,      Level::Default,ErrType::InvalidInput },
        { ERROR_DISK_FULL,           Level::Fatal,  ErrType::Io },
        { ERROR_WRITE_PROTECT,       Level::Retry,  ErrType::Io },
        { ERROR_WRITE_FAULT,         Level::Retry,  ErrType::Io },
        { ERROR_READ_FAULT,          Level::Retry,  ErrType::Io },
        { ERROR_CRC,                 Level::Retry,  ErrType::Io },
        { ERROR_IO_DEVICE,           Level::Retry,  ErrType::Io },
        { ERROR_FUNCTION_FAILED,     Level::Retry,  ErrType::Unexpected },
        { 999999,                    Level::Default,ErrType::Default }
    };

    for(const auto &c : cases){
        Error e = Error::fromLastWinError(c.code, "context", 0);
        EXPECT_EQ(e.level(), c.level) << "Code: " << c.code;
        EXPECT_EQ(e.type(), c.type) << "Code: " << c.code;
        auto wd = e.whichWindow();
        EXPECT_TRUE(wd.name.empty());
    }
}
#endif

