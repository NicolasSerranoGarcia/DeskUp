#include "desk_up_error.h"

DeskUp::Error DeskUp::Error::fromLastWinError(DWORD error, std::string_view context, std::optional<unsigned int> tries){
    Level   lvl = Level::Default;
    ErrType typ = ErrType::Unexpected;

    DWORD code = error;

    switch (code) {
        case ERROR_ACCESS_DENIED:
        case ERROR_PRIVILEGE_NOT_HELD:
            lvl = Level::Fatal; typ = ErrType::AccessDenied; break;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            lvl = Level::Fatal; typ = ErrType::InsufficientMemory; break;

        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
            lvl = Level::Retry; typ = ErrType::SharingViolation; break;

        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_NAME:
        case ERROR_FILENAME_EXCED_RANGE:
            lvl = Level::Fatal; typ = ErrType::InvalidInput; break;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            lvl = Level::Default; typ = ErrType::InvalidInput; break;

        case ERROR_DISK_FULL:
            lvl = Level::Fatal; typ = ErrType::Io; break;
        case ERROR_WRITE_PROTECT:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
            lvl = Level::Retry; typ = ErrType::Io; break;

        default:
            lvl = Level::Default; typ = ErrType::Default; break;
    }

    std::string msg = getSystemErrorMessageWindows(code, context);
    unsigned int t = tries.value_or(0);
    return Error(lvl, typ, t, std::move(msg));
}

DeskUp::Error DeskUp::Error::fromLastWinError(std::string_view context, std::optional<unsigned int> tries){
    Level   lvl = Level::Default;
    ErrType typ = ErrType::Unexpected;

    DWORD code = GetLastError();

    switch (code) {
        case ERROR_ACCESS_DENIED:
        case ERROR_PRIVILEGE_NOT_HELD:
            lvl = Level::Fatal; typ = ErrType::AccessDenied; break;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            lvl = Level::Fatal; typ = ErrType::InsufficientMemory; break;

        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
            lvl = Level::Retry; typ = ErrType::SharingViolation; break;

        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_NAME:
        case ERROR_FILENAME_EXCED_RANGE:
            lvl = Level::Fatal; typ = ErrType::InvalidInput; break;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            lvl = Level::Default; typ = ErrType::InvalidInput; break;

        case ERROR_DISK_FULL:
            lvl = Level::Fatal; typ = ErrType::Io; break;
        case ERROR_WRITE_PROTECT:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
            lvl = Level::Retry; typ = ErrType::Io; break;

        default:
            lvl = Level::Default; typ = ErrType::Default; break;
    }

    std::string msg = getSystemErrorMessageWindows(code, context);
    unsigned int t = tries.value_or(0);
    return Error(lvl, typ, t, std::move(msg));
}