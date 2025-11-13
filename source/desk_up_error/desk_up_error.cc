#include "desk_up_error.h"

#ifdef _WIN32

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
            lvl = Level::Skip; typ = ErrType::InvalidInput; break;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            lvl = Level::Skip; typ = ErrType::InvalidInput; break;

        case ERROR_DISK_FULL:
            lvl = Level::Fatal; typ = ErrType::Io; break;
        case ERROR_WRITE_PROTECT:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
            lvl = Level::Retry; typ = ErrType::Io; break;

        case ERROR_FUNCTION_FAILED:
            lvl = Level::Retry; typ = ErrType::Unexpected; break;

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
            lvl = Level::Skip; typ = ErrType::InvalidInput; break;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            lvl = Level::Skip; typ = ErrType::InvalidInput; break;

        case ERROR_DISK_FULL:
            lvl = Level::Fatal; typ = ErrType::Io; break;
        case ERROR_WRITE_PROTECT:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
            lvl = Level::Retry; typ = ErrType::Io; break;

        case ERROR_FUNCTION_FAILED:
            lvl = Level::Retry; typ = ErrType::Unexpected; break;

        default:
            lvl = Level::Default; typ = ErrType::Default; break;
    }

    std::string msg = getSystemErrorMessageWindows(code, context);
    unsigned int t = tries.value_or(0);
    return Error(lvl, typ, t, std::move(msg));
}

#endif

DeskUp::Error DeskUp::Error::fromSaveError(int e){
     switch (e) {
        case 1:
            return Error(Level::Info, ErrType::None, e, "windowDesc::saveTo: operation successful");

        case -1:
            return Error(Level::Error, ErrType::InvalidInput, e, "windowDesc::saveTo: file path is empty");

        case -2:
            return Error(Level::Error, ErrType::Io, e, "windowDesc::saveTo: could not open file");

        case -3:
            return Error(Level::Error, ErrType::AccessDenied, e, "windowDesc::saveTo: permission denied");

        case -4:
            return Error(Level::Error, ErrType::FileNotFound, e, "windowDesc::saveTo: file or directory not found");

        case -5:
            return Error(Level::Fatal, ErrType::DiskFull, e, "windowDesc::saveTo: disk full or no space left");

        case -6:
            return Error(Level::Error, ErrType::Unexpected, e, "windowDesc::saveTo: unknown I/O error");

        default:
            return Error(Level::Warning, ErrType::Default, e, "windowDesc::saveTo: unrecognized error code");
    }
}