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
		case ERROR_TOO_MANY_OPEN_FILES:
            lvl = DeskUp::Level::Fatal; typ = DeskUp::ErrType::InsufficientMemory; break;

        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_NAME:
        case ERROR_FILENAME_EXCED_RANGE:
		case ERROR_BAD_FORMAT:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::InvalidInput; break;

		case ERROR_DLL_NOT_FOUND:
            lvl = DeskUp::Level::Warning; typ = DeskUp::ErrType::PolicyUpdated; break;

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

		case ERROR_INVALID_WINDOW_STYLE:
			lvl = DeskUp::Level::Info; typ = DeskUp::ErrType::ResourceBusy; break;

		case ERROR_ACCESS_DISABLED_BY_POLICY:
            lvl = DeskUp::Level::Skip; typ = DeskUp::ErrType::Default; break;


		case ERROR_INVALID_HANDLE:
		case ERROR_INVALID_WINDOW_HANDLE:
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

        case 1: // SaveErrorCode::SAVE_SUCCESS
            return Error(Level::Info, ErrType::None, e, "saveTo|success");

        case -1: // SaveErrorCode::ERR_EMPTY_PATH
            return Error(Level::Error, ErrType::InvalidInput, e, "saveTo|path_empty");

        case -2: // SaveErrorCode::ERR_FILE_NOT_OPEN
            return Error(Level::Error, ErrType::Io, e, "saveTo|file_unopened");

        case -3: // SaveErrorCode::ERR_NO_PERMISSION
            return Error(Level::Error, ErrType::AccessDenied, e, "saveTo|permission_denied");

        case -4: // SaveErrorCode::ERR_FILE_NOT_FOUND
            return Error(Level::Error, ErrType::FileNotFound, e, "saveTo|not_found");

        case -5: // SaveErrorCode::ERR_DISK_FULL
            return Error(Level::Fatal, ErrType::DiskFull, e, "saveTo|no_space_left");

        case -6: // SaveErrorCode::ERR_UNKNOWN
            return Error(Level::Error, ErrType::Unexpected, e, "saveTo|unknown_error");

        default: // other
            return Error(Level::Warning, ErrType::Default, e, "saveTo|unrecognized_error");
    }
}