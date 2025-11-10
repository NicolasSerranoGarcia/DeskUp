#include <gtest/gtest.h>

#include "desk_up_error.h"
#include "desk_up_error_gui_converter.h"
#include <QMessageBox>
#include <QString>

using namespace DeskUp;
using namespace DeskUp::UI;

// Test mapLevel for all Level values.
TEST(DeskUpErrorGuiConverterTest, mapLevelFatal){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Fatal);
    EXPECT_EQ(title, QString("Critical Error"));
    EXPECT_EQ(icon, QMessageBox::Icon::Critical);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelError){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Error);
    EXPECT_EQ(title, QString("Error"));
    EXPECT_EQ(icon, QMessageBox::Icon::Critical);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelWarning){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Warning);
    EXPECT_EQ(title, QString("Warning"));
    EXPECT_EQ(icon, QMessageBox::Icon::Warning);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelInfo){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Info);
    EXPECT_EQ(title, QString("Information"));
    EXPECT_EQ(icon, QMessageBox::Icon::Information);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelRetry){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Retry);
    EXPECT_EQ(title, QString("Retry"));
    EXPECT_EQ(icon, QMessageBox::Icon::Warning);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelDefault){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::Default);
    EXPECT_EQ(title, QString("DeskUp"));
    EXPECT_EQ(icon, QMessageBox::Icon::NoIcon);
}

TEST(DeskUpErrorGuiConverterTest, mapLevelNone){
    auto [title, icon] = ErrorAdapter::mapLevel(Level::None);
    EXPECT_EQ(title, QString("DeskUp"));
    EXPECT_EQ(icon, QMessageBox::Icon::NoIcon);
}

// Test getUserMessage for all mapped ErrType values.
TEST(DeskUpErrorGuiConverterTest, getUserMessageAccessDenied){
    Error err(Level::Error, ErrType::AccessDenied, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("You do not have permission to perform this operation."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageFileNotFound){
    Error err(Level::Error, ErrType::FileNotFound, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("The specified file or directory could not be found."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageDiskFull){
    Error err(Level::Fatal, ErrType::DiskFull, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("There is not enough disk space to complete the operation."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageInvalidInput){
    Error err(Level::Error, ErrType::InvalidInput, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("One or more provided parameters are invalid."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageIo){
    Error err(Level::Error, ErrType::Io, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("An input/output error occurred while accessing the filesystem."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageInsufficientMemory){
    Error err(Level::Fatal, ErrType::InsufficientMemory, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("The system ran out of memory."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageTimeout){
    Error err(Level::Error, ErrType::Timeout, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("The operation timed out and was cancelled."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageUnexpected){
    Error err(Level::Error, ErrType::Unexpected, 0, "internal");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("An unexpected error occurred."));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageFallback){
    Error err(Level::Warning, ErrType::NotImplemented, 0, "feature not yet available");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("feature not yet available"));
}

TEST(DeskUpErrorGuiConverterTest, getUserMessageFallbackDefault){
    Error err(Level::Default, ErrType::Default, 0, "some generic error");
    QString msg = ErrorAdapter::getUserMessage(err);
    EXPECT_EQ(msg, QString("some generic error"));
}

// showError might be tested with specific qt testing libraries in the future bc it requires a QApplication instance