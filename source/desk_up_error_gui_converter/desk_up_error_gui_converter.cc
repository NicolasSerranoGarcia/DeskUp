#include "desk_up_error_gui_converter.h"

namespace DeskUp::UI {

int ErrorAdapter::showError(const DeskUp::Error& err) {
    const auto [title, style] = mapLevel(err.level());
    return wxMessageBox(getUserMessage(err), title, style);
}

std::pair<wxString, long> ErrorAdapter::mapLevel(Level lvl) {
    switch (lvl) {
        case Level::Fatal:   return { "Critical Error", wxICON_ERROR | wxOK };
        case Level::Error:   return { "Error", wxICON_ERROR | wxOK };
        case Level::Warning: return { "Warning", wxICON_WARNING | wxOK };
        case Level::Info:    return { "Information", wxICON_INFORMATION | wxOK };
        case Level::Retry:   return { "Retry", wxICON_WARNING | wxOK };
        default:             return { "DeskUp", wxICON_NONE | wxOK };
    }
}

wxString ErrorAdapter::getUserMessage(const DeskUp::Error& err) {
    switch (err.type()) {
        case ErrType::AccessDenied:
            return "You do not have permission to perform this operation.";
        case ErrType::FileNotFound:
            return "The specified file or directory could not be found.";
        case ErrType::DiskFull:
            return "There is not enough disk space to complete the operation.";
        case ErrType::InvalidInput:
            return "One or more provided parameters are invalid.";
        case ErrType::Io:
            return "An input/output error occurred while accessing the filesystem.";
        case ErrType::InsufficientMemory:
            return "The system ran out of memory.";
        case ErrType::Timeout:
            return "The operation timed out and was cancelled.";
        case ErrType::Unexpected:
            return "An unexpected error occurred.";
        default:
            return wxString::FromUTF8(err.what());
    }
}

}