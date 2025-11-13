#include "desk_up_error.h"

#ifdef _WIN32

static std::pair<DeskUp::Level, DeskUp::ErrType> getErrType(const DWORD code) {

	DeskUp::Level lvl;
	DeskUp::ErrType typ;

	switch (code) {
        case ERROR_ACCESS_DENIED:
        case ERROR_PRIVILEGE_NOT_HELD:
            lvl = DeskUp::Level::Fatal; typ = DeskUp::ErrType::AccessDenied; break;

        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            lvl = DeskUp::Level::Fatal; typ = DeskUp::ErrType::InsufficientMemory; break;

        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
            lvl = DeskUp::Level::Retry; typ = DeskUp::ErrType::SharingViolation; break;

        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_NAME:
        case ERROR_FILENAME_EXCED_RANGE:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::InvalidInput; break;

        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::InvalidInput; break;

        case ERROR_DISK_FULL:
            lvl = DeskUp::Level::Fatal; typ = DeskUp::ErrType::Io; break;
        case ERROR_WRITE_PROTECT:
        case ERROR_WRITE_FAULT:
        case ERROR_READ_FAULT:
        case ERROR_CRC:
        case ERROR_IO_DEVICE:
            lvl = DeskUp::Level::Retry; typ = DeskUp::ErrType::Io; break;

        case ERROR_FUNCTION_FAILED:
            lvl = DeskUp::Level::Retry; typ = DeskUp::ErrType::Unexpected; break;

		case ERROR_ACCESS_DISABLED_BY_POLICY:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::Default; break;


		case ERROR_INVALID_HANDLE:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::ConnectionRefused; break;

        default:
            lvl = DeskUp::Level::Default; typ = DeskUp::ErrType::Default; break;
    }

	return {lvl, typ};
}

DeskUp::Error DeskUp::Error::fromLastWinError(DWORD error, std::string_view context, std::optional<unsigned int> tries){


	auto [lvl, typ] = getErrType(error);

    std::string msg = getSystemErrorMessageWindows(error, context);
    unsigned int t = tries.value_or(0);
    return Error(lvl, typ, t, std::move(msg));
}

DeskUp::Error DeskUp::Error::fromLastWinError(std::string_view context, std::optional<unsigned int> tries){

	auto code = GetLastError();

	auto [lvl, typ] = getErrType(GetLastError());

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