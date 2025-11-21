#include "desk_up_win.h"

#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <expected>
#include <shlobj.h>

#include <tlhelp32.h>
#include <shellapi.h>

#include "backend_utils.h"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

template<typename F>
static DeskUp::Status retryOp(F&& f, std::string_view ctx, unsigned int maxAttempts = 3, std::chrono::milliseconds firstDelay = 50ms)
{
    auto delay = firstDelay;
    for (unsigned int i = 0; i < maxAttempts; i++) {
        if (f()){
            return {};
        }

        DeskUp::Error e = DeskUp::Error::fromLastWinError(ctx, i+1);
        if (e.isFatal() || e.isSkippable() || e.isWarning()) {
            return std::unexpected(std::move(e));
        }

        std::this_thread::sleep_for(delay);
        delay *= 2;
    }

    return std::unexpected(DeskUp::Error::fromLastWinError(ctx, maxAttempts));
}


static std::unique_ptr<HWND> desk_up_hwnd = nullptr;

struct windowData{
    HWND hwnd;
};

DeskUpWindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice,
    WIN_isAvailable
};

bool WIN_isAvailable() noexcept {
    #ifdef _WIN32
        return true;
    #endif

    return false;
}

static windowData * getWindowData(DeskUpWindowDevice * dev){
    auto* data = static_cast<windowData*>(dev->internalData);
	return data;
}

static HWND WIN_getDeskUpHWND(){

    HWND deskUpWindow = nullptr;

    // For now, default desktop
    HDESK desk = NULL;

    auto isDeskUp = [](HWND hwnd, LPARAM lparam){
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        DWORD deskUpPid = GetCurrentProcessId();
        if (pid == deskUpPid) {
            HWND * h = reinterpret_cast<HWND*>(lparam);
            *h = hwnd;
            return FALSE;
        }
        return TRUE;
    };

    EnumDesktopWindows(desk, isDeskUp, reinterpret_cast<LPARAM>(&deskUpWindow));

    return deskUpWindow;
}

DeskUpWindowDevice WIN_CreateDevice() noexcept{

    //Initialize COM (Object Component Model), which may be used by the shell when recovering the windows from the files.
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    // Set function pointers and initialize internal data
    desk_up_hwnd = std::make_unique<HWND>(WIN_getDeskUpHWND());

    DeskUpWindowDevice device;

    device.getWindowHeight = WIN_getWindowHeight;
    device.getWindowWidth  = WIN_getWindowWidth;
    device.getWindowXPos   = WIN_getWindowXPos;
    device.getWindowYPos   = WIN_getWindowYPos;
    device.getPathFromWindow = WIN_getPathFromWindow;
    device.getAllOpenWindows   = WIN_getAllOpenWindows;
    device.getDeskUpPath   = WIN_getDeskUpPath;
    device.loadWindowFromPath = WIN_loadProcessFromPath;
    device.recoverSavedWindow = WIN_recoverSavedWindow;
    device.resizeWindow    = WIN_resizeWindow;
    device.closeProcessFromPath = WIN_closeProcessFromPath;
	device.DestroyDevice = WIN_destroyDevice;

    device.internalData = (void *) new windowData();

    return device;
}

void WIN_destroyDevice(DeskUpWindowDevice* _this) noexcept {
	delete getWindowData(_this);
}

DeskUp::Result<std::string> WIN_getDeskUpPath() noexcept{
    PWSTR wpath = nullptr;
    std::string base;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &wpath)) && wpath){
        base = WideStringToUTF8(wpath);
        CoTaskMemFree(wpath);
    } else {

        if (const char* appdata = std::getenv("APPDATA"); appdata && *appdata){
            base = appdata;
        } else {

            char buf[MAX_PATH]{};
            DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
            if (n) {
                std::string exeDir(buf, buf + n);
                size_t pos = exeDir.find_last_of("\\/");
                if (pos != std::string::npos) exeDir.resize(pos);
                base = exeDir;
            } else {
                base = ".";
            }
        }
    }

    std::filesystem::path p = std::filesystem::path(base) / "DeskUp";
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    return p.string();
}


DeskUp::Result<int> WIN_getWindowXPos(DeskUpWindowDevice* _this) noexcept {
    const auto* data = getWindowData(_this);

	if(!data){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_getWindowXPos|no_device"));
	}

    if(!IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_getWindowXPos|no_hwnd"));
    }

	//brackets necessary to initialize the struct (memset 0)
    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);

    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; }, "WIN_getWindowXPos>GetWindowInfo|");
    if (!r){
		return std::unexpected(std::move(r.error()));
	}

    return static_cast<int>(wi.rcWindow.left);
}

DeskUp::Result<int> WIN_getWindowYPos(DeskUpWindowDevice* _this) noexcept {
    const auto* data = getWindowData(_this);

	if(!data){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_getWindowYPos|no_device"));
	}

    if(!IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_getWindowYPos|no_hwnd"));
    }

	//brackets necessary to initialize the struct (memset 0)
    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);

    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; }, "WIN_getWindowYPos>GetWindowInfo|");
    if (!r){
		return std::unexpected(std::move(r.error()));
	}

    return static_cast<int>(wi.rcWindow.top);
}

DeskUp::Result<unsigned int> WIN_getWindowWidth(DeskUpWindowDevice* _this) noexcept{
    const auto* data = getWindowData(_this);

	if(!data){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_getWindowWidth|no_device"));
	}

    if(!IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_getWindowWidth|no_hwnd"));
    }

	//brackets necessary to initialize the struct (memset 0)
    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);

    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; }, "WIN_getWindowWidth>GetWindowInfo|");
    if (!r){
		return std::unexpected(std::move(r.error()));
	}

    return static_cast<unsigned int>(wi.rcWindow.left - wi.rcWindow.right);
}

DeskUp::Result<unsigned int> WIN_getWindowHeight(DeskUpWindowDevice* _this) noexcept {
    const auto* data = getWindowData(_this);

	if(!data){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_getWindowHeight|no_device"));
	}

    if(!IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_getWindowHeight|no_hwnd"));
    }

	//brackets necessary to initialize the struct (memset 0)
    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);

    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; }, "WIN_getWindowHeight>GetWindowInfo|");
    if (!r){
		return std::unexpected(std::move(r.error()));
	}

    return static_cast<unsigned int>(wi.rcWindow.top - wi.rcWindow.bottom);
}


DeskUp::Result<fs::path> WIN_getPathFromWindow(DeskUpWindowDevice* _this) noexcept{
	const auto* data = getWindowData(_this);

	if(!data){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_getWindowWidth|no_device"));
	}

    if(!IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_getWindowWidth|no_hwnd"));
    }

    DWORD pid = 0;

    auto r = retryOp([&]{ return (GetWindowThreadProcessId(data->hwnd, &pid) != 0) && (pid != 0); }, "WIN_getPathFromWindow>GetWindowThreadProcessId|");
    if (!r){
		return std::unexpected(std::move(r.error()));
	}

    HANDLE processHandle = nullptr;
    r = retryOp([&]{
			SetLastError(0);
			processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_QUERY_INFORMATION, FALSE, pid);
			//TODO: even though the ERROR_ACCESS_DENIED is always returned when the program doesn't have enough priviledges or the destination
			//window is WUP (windows apps), it can also return it because DeskUp isn't given priviledges, even though it might be able to access
			//the information of the window. For this, you might check permissions when DeskUp starts, and alert the user to give it permissions,
			//or it might malfunction. Supposing this, the function openProcess can only return access denied if the window is inaccessible (regardless
			//of the permissions)
			auto err = GetLastError();
			if(!processHandle && (err == ERROR_ACCESS_DENIED)){
				SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);
				return false;
			}

			DWORD exitCode = 0;
			//if failed, will set INVALID_HANDLE and return false
			if (GetExitCodeProcess(processHandle, &exitCode)) {
    			if (exitCode == STILL_ACTIVE) {
					return true;
				} else {
					SetLastError(ERROR_INVALID_HANDLE);
					return false;
    			}
			}

			auto error = GetLastError();

			if(error == ERROR_ACCESS_DENIED){
				SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);
				return false;
			}

			SetLastError(error);
			return false;

		},
		"WIN_getPathFromWindow>OpenProcess|");

	if (!r){
		return std::unexpected(std::move(r.error()));
	}

    std::string result;
    DWORD capacity = 512;
    std::vector<wchar_t> wbuf(capacity);

    while(true) {
        DWORD size = capacity;
        if (QueryFullProcessImageNameW(processHandle, 0, wbuf.data(), &size)) {
            result = WideStringToUTF8(wbuf.data());
            break;
        }
        DWORD err = GetLastError();
        if (err == ERROR_INSUFFICIENT_BUFFER) {
            capacity *= 2;
            wbuf.assign(capacity, L'\0');
            continue;
        } else {
            CloseHandle(processHandle);
            return std::unexpected(DeskUp::Error::fromLastWinError(
                err, "WIN_getPathFromWindow>QueryFullProcessImageNameW|"));
        }
    }

    CloseHandle(processHandle);
    return fs::path(result);
}

static std::string WIN_getNameFromPath(const fs::path& path) noexcept{
    static int unnamedWindowNum = 0;
    if (path.empty()) {
        return "window" + std::to_string(unnamedWindowNum++);
    }

    try {
        return path.stem().string();
    } catch (...) {
		return "window" + std::to_string(unnamedWindowNum++);
	}
}


struct saveWindowParams {
        DeskUpWindowDevice* dev;
        std::vector<windowDesc>* res;
        DeskUp::Error* err;
};

static BOOL CALLBACK WIN_CreateAndSaveWindowProc(HWND hwnd, LPARAM lparam) noexcept{

    if (!IsWindowVisible(hwnd)){
		return TRUE;
	}

	//brackets necessary to initialize the struct (memset 0)
    RECT r{};
    if (!GetWindowRect(hwnd, &r)){
		return TRUE;
	}

    if ((r.right - r.left) == 0 || (r.bottom - r.top) == 0){
		return TRUE;
	}

    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    if (strlen(title) == 0){
		return TRUE;
	}

	if (*desk_up_hwnd.get() == hwnd) {
        return TRUE;
    }

    auto* parameters = reinterpret_cast<saveWindowParams*>(lparam);
    if (!parameters || !parameters->err){
		//if the parameters didn't get passed, we can't set the error inside to inform, so just return. This is very improbable
		return FALSE;
	}

    auto& err = *parameters->err;

	if(!parameters->res){
		err = DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "WIN_CreateAndSaveWindowProc|no_parameter");
		return FALSE;
	}

    auto& windows = *parameters->res;

	if(!parameters->dev || !parameters->dev->internalData) {
		err = DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_CreateAndSaveWindowProc|no_device");
        return FALSE;
    }

    DeskUpWindowDevice* dev = parameters->dev;

	windowData * data = getWindowData(dev);
    data->hwnd = hwnd;

    windowDesc window;

	static bool levelErrorHappened = false;

	if (auto res = WIN_getPathFromWindow(dev); res.has_value()) {
        window.pathToExec = std::move(res.value());
    } else {
		err = std::move(res.error());

		if(err.isFatal()){
			data->hwnd = nullptr;
			return FALSE;
		}

		//hwnd might have gone invalid
		if(err.isSkippable() && !IsWindow(data->hwnd)){
			//can't know the name of the window if it failed
			std::cout << "Window Skipped" << std::endl;
			data->hwnd = nullptr;
			return TRUE;
		}

		//The data is corrupt or invalid. Skip the window
		if(err.isError()){
			data->hwnd = nullptr;

			if(levelErrorHappened){
				levelErrorHappened = false;
				return FALSE;
			}

			levelErrorHappened = true;
			return TRUE;
		}

		return TRUE;
    }

	//can't fail
    window.name = WIN_getNameFromPath(window.pathToExec);


    if (auto res = WIN_getWindowXPos(dev); res.has_value()) {
        window.x = std::move(res.value());
    } else {
        err = std::move(res.error());

		if(err.isFatal()){
			data->hwnd = nullptr;
			return FALSE;
		}

		//hwnd might have gone invalid
		if(err.isSkippable() && !IsWindow(data->hwnd)){
			std::cout << "Window Skipped: " << window.name << std::endl;
			data->hwnd = nullptr;
			return TRUE;
		}

		//The data is corrupt or invalid. Skip the window
		if(err.isError()){
			data->hwnd = nullptr;

			if(levelErrorHappened){
				levelErrorHappened = false;
				return FALSE;
			}

			levelErrorHappened = true;
			return TRUE;
		}

		return TRUE;
    }

    if (auto res = WIN_getWindowYPos(dev); res.has_value()) {
        window.y = std::move(res.value());
    } else {
        err = std::move(res.error());

		if(err.isFatal()){
			data->hwnd = nullptr;
			return FALSE;
		}

		//hwnd might have gone invalid
		if(err.isSkippable() && !IsWindow(data->hwnd)){
			std::cout << "Window Skipped: " << window.name << std::endl;
			data->hwnd = nullptr;
			return TRUE;
		}

		//The data is corrupt or invalid. Skip the window
		if(err.isError()){
			data->hwnd = nullptr;

			if(levelErrorHappened){
				levelErrorHappened = false;
				return FALSE;
			}

			levelErrorHappened = true;
			return TRUE;
		}

		return TRUE;
    }

    if (auto res = WIN_getWindowWidth(dev); res.has_value()) {
        window.w = std::move(res.value());
    } else {
        err = std::move(res.error());

		if(err.isFatal()){
			data->hwnd = nullptr;
			return FALSE;
		}

		//hwnd might have gone invalid
		if(err.isSkippable() && !IsWindow(data->hwnd)){
			std::cout << "Window Skipped: " << window.name << std::endl;
			data->hwnd = nullptr;
			return TRUE;
		}

		//The data is corrupt or invalid. Skip the window
		if(err.isError()){
			data->hwnd = nullptr;

			if(levelErrorHappened){
				levelErrorHappened = false;
				return FALSE;
			}

			levelErrorHappened = true;
			return TRUE;
		}

		return TRUE;
    }

    if (auto res = WIN_getWindowHeight(dev); res.has_value()) {
        window.h = std::move(res.value());
    } else {
        err = std::move(res.error());

		if(err.isFatal()){
			data->hwnd = nullptr;
			return FALSE;
		}

		//hwnd might have gone invalid
		if(err.isSkippable() && !IsWindow(data->hwnd)){
			std::cout << "Window Skipped: " << window.name << std::endl;
			data->hwnd = nullptr;
			return TRUE;
		}

		//The data is corrupt or invalid. Skip the window
		if(err.isError()){
			data->hwnd = nullptr;

			if(levelErrorHappened){
				levelErrorHappened = false;
				return FALSE;
			}

			levelErrorHappened = true;
			return TRUE;
		}

		return TRUE;
    }

    data->hwnd = nullptr;

    windows.push_back(std::move(window));
    return TRUE;
}

DeskUp::Result<std::vector<windowDesc>> WIN_getAllOpenWindows(DeskUpWindowDevice* _this) noexcept{
    std::vector<windowDesc> windows;
    DeskUp::Error error{};


    saveWindowParams p{ _this, &windows, &error };

    HDESK desktop = NULL;

    if (!EnumDesktopWindows(desktop, WIN_CreateAndSaveWindowProc, reinterpret_cast<LPARAM>(&p))) {
        if (error.level() == DeskUp::Level::Fatal){
			return std::unexpected(std::move(error));
		}

		if(error.level() == DeskUp::Level::Error){
			//this only happens when something happened to the device or the windowData. For this, return an unexpected error
			return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::Unexpected, 0u, "WIN_getAllOpenWindows>EnumDesktopWindows|device_corrupt"));
		}

		//Skippable errors should not get there. Anything else besides fatal and error will get bubbled up as a warning for the user

        return std::unexpected(DeskUp::Error(DeskUp::Level::Warning, DeskUp::ErrType::Unexpected, 0u, "WIN_getAllOpenWindows>EnumDesktopWindows|"));
    }

    return windows;
}

static void stripEndlChars(std::string& s){
    if(!s.empty() && (s.back() == '\r' || s.back() == '\n')){
		s.pop_back();
	}
}

DeskUp::Result<windowDesc> WIN_recoverSavedWindow(DeskUpWindowDevice*, const fs::path& path) noexcept{
	//this function expects to receive the path from DeskUpWindow::restoreWindows, which already checks the validity of the path. Still, do it here
    std::error_code fec;
    if (!fs::exists(path, fec) || !fs::is_regular_file(path, fec)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "WIN_recoverSavedWindow|no_file_" + path.string()));
    }

    if (fec) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::Io, 0, "WIN_recoverSavedWindow|fs_error_" + fec.message()));
    }

    constexpr int kMaxAttempts = 3;
    std::chrono::milliseconds delay{50};
    std::ifstream f;

	//try reopening the file if it is not opened
    for (int attempt = 1; attempt <= kMaxAttempts; attempt++) {
        f.open(path, std::ios::in);
        if (f.is_open()){
			break;
		}

        if (attempt == kMaxAttempts) {
            return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::Io, attempt,"WIN_recoverSavedWindow|file_unopen_" + path.string()));
        }
        std::this_thread::sleep_for(delay);
        delay *= 2;
    }

    auto parse_int = [](const std::string& str, const char*) -> std::optional<int> {
        try {
            size_t pos = 0;
			//can throw invalid_argument if there could not be any conversion (there are no numbers in the string or the data is corrupt)
            int val = std::stoi(str, &pos);
			//the string passed should be solely composed of numeric chars (check for endl or tabs is checked before calling this function)
            if (pos != str.size()){
				return std::nullopt;
			}
            return val;
        } catch (...) {
			//this only happens if stoi failed
			return std::nullopt;
		}
    };

	std::string s;

	//calls default constructor
    windowDesc w;
	int i = 0;

	//relies on: w,h > 0, no final endl or EOF, just 5 lines
    while (std::getline(f, s)) {
        stripEndlChars(s);
        switch (i) {
            case 0:
                w.pathToExec = s;
                w.name = WIN_getNameFromPath(s);
                break;
            case 1: {
                auto v = parse_int(s, "x");
                if (!v) {
					return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0, "WIN_recoverSavedWindow|invalid_read_x"));
				}
                w.x = *v;
                break;
            }
            case 2: {
                auto v = parse_int(s, "y");
                if (!v){
					return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0, "WIN_recoverSavedWindow|invalid_read_y"));
				}
                w.y = *v;
                break;
            }
            case 3: {
                auto v = parse_int(s, "w");
                if (!v){
					return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0, "WIN_recoverSavedWindow|invalid_read_w"));
				}
                w.w = *v;
                break;
            }
            case 4: {
                auto v = parse_int(s, "h");
                if (!v) {
					return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0, "WIN_recoverSavedWindow|invalid_read_h"));
				}
                w.h = *v;
                break;
            }
            default:
                break;
        }
        i++;
    }

    if (i < 5) {
		//if there is less than 5 lines, this is an i/o error, not a read error, because when the file was being written was done wrongly.
		//This is very rare, but it might identify with a write error on a specific value of any of the windowDesc info that was written.
		//For now, just return an error
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::Io, 0, "WIN_recoverSavedWindow|previous_write_failed"));
    }

    return w;
}

//Helper for WIN_loadProcessFromPath. It just returns the specified handle for the pid. It is used to get the hwnd of the launched window
//to resize it later
//It is useful to understand how windows organizes windows:
//Process (PID=4321) -> this is what is returned in the second parameter of getWindowThreadProcessId
// │
// ├── Thread A (TID=100) -> this is the return of getWindowThreadProcessId
// │     ├── Window #1 (HWND=A1) -> this is what you pass to getWindowThreadProcessId
// │     └── Window #2 (HWND=A2)
// │
// ├── Thread B (TID=200)
// │     └── Window #3 (HWND=B1)
// │
// └── Thread C (TID=300)
//       └── No windows
static HWND WIN_FindMainWindow(DWORD pid, int timeoutMs = 300) {
    HWND hwndFound = nullptr;
    auto enumCallback = [](HWND hwnd, LPARAM lParam) -> BOOL {
        auto data = reinterpret_cast<std::pair<DWORD, HWND*>*>(lParam);
        DWORD winPid;
		//get the process pid of the handle of the window (the general pid of that window)
        GetWindowThreadProcessId(hwnd, &winPid);

		//it might be possible that this check give false positives, because there might be a window that we want to search
		//which happens to: be part of the same app (has the same pid), be independent or is a child (it has parent, but not owner) and is also visible
        if ((winPid == data->first/*If the window is part of the app we are looking for */
			&& GetWindow(hwnd, GW_OWNER) == nullptr) /*returns the owner window of the specific window we passed. If it is nullptr it usually means the top-level*/
			&& GetAncestor(hwnd, GA_ROOT) == nullptr /*Returns the parent (not the owner) window of the window we passed. Main windows do not have a parent*/
			&& IsWindowVisible(hwnd) /*If the window was set to be visible*/) {
            *data->second = hwnd;
            return FALSE;
        }
        return TRUE;
    };

    const int step = 100;
    for (int waited = 0; waited < timeoutMs && !hwndFound; waited += step) {
        std::pair<DWORD, HWND*> data{ pid, &hwndFound };
        EnumWindows(enumCallback, reinterpret_cast<LPARAM>(&data));
        if (hwndFound){
			break;
		}
        Sleep(step);
    }
    return hwndFound;
}

DeskUp::Status WIN_loadProcessFromPath(DeskUpWindowDevice* _this, const fs::path& path) noexcept {

	auto data = getWindowData(_this);
	data->hwnd = nullptr;

    if (path.empty()) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "WIN_loadProcessFromPath|no_file_" + path.string()));
    }

	if(!fs::exists(path)){
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::FileNotFound, 0, "WIN_loadProcessFromPath|invalid_file_" + path.string()));
	}

    if (!_this || !_this->internalData) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_loadProcessFromPath|no_device"));
    }

    SHELLEXECUTEINFO ShExecInfo{};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOASYNC;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = nullptr;
    ShExecInfo.lpFile = path.c_str();
    ShExecInfo.lpParameters = nullptr;
    ShExecInfo.lpDirectory = nullptr;
    ShExecInfo.nShow = SW_NORMAL;
    ShExecInfo.hInstApp = NULL;

	//this operation is just used to get the handle to the window we just opened. It does not check that the window really opened. It revolves around
	//executing ShellExecuteEx, which just tells the shell to execute whatever we passed. For this, this just should check that the credentials to execute
	//are correct.
    auto status = retryOp([&]{
        SetLastError(0);
        BOOL ok = ShellExecuteEx(&ShExecInfo);
        DWORD err = GetLastError();
        if (!ok) {
			//any of privilege errors default to disabled by policy, as we stated in earlier functions, we assume the user has given us all the necessary
			//privileges, and that if the function fails and returns this type of errors it is because the window is protected by windows and we cannot access it.
			if(err == ERROR_ACCESS_DENIED || err == ERROR_PRIVILEGE_NOT_HELD || err == ERROR_ELEVATION_REQUIRED){
				SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);
				return false;
			}

			//file errors get rethrown, as the error converter already maps them correctly
			if(err == ERROR_FILE_NOT_FOUND || err == ERROR_INVALID_NAME || err == ERROR_PATH_NOT_FOUND || err == ERROR_BAD_FORMAT){
				SetLastError(err);
				return false;
			}

			//system errors should be fatal and end execution, so just rethrow and let the converter handle it
			if(err == ERROR_NOT_ENOUGH_MEMORY || err == ERROR_OUTOFMEMORY || err == ERROR_TOO_MANY_OPEN_FILES){
				SetLastError(err);
				return false;
			}

			//This is improbable but can happen and should only happen if any of the dependencies of DeskUp has been updated, and we haven't noticed.
			//This could happen with indirect dependencies of Qt or other libraries, so it is worth calling it out. Still, when packaging te executable
			//dependencies get called out dynamically and depending on the executable, so the workflows or new releases will fail.
			if(err == ERROR_DLL_NOT_FOUND){
				SetLastError(err);
				return false;
			}


            err = ERROR_FUNCTION_FAILED;
            SetLastError(err);
			return false;
        }

		//if there is no error, just return true
        return true;
    },
	"WIN_loadProcessFromPath>ShellExecuteEx|");

	//the windows errors set inside the callback get translated here
    if (!status) {
        return std::unexpected(std::move(status.error()));
    }

	//at this point, the handle to the process is required  (and is expected) to be correct, and the only errors that might arise come from
	//the application itself.

    if (ShExecInfo.hProcess) {
		//wait for the application to finish seting up, before returning the handle. This prevents invalid accesses or missed searches of the
		//window we just created.
		//The function can finish earlier than the specified time if the app loads faster than the specified time, so there is no need to
		//check in time intervals for the finished window.

		DWORD res = WAIT_FAILED;


		while(res != 0){
			//WaitForInputIdle returns exit code 0 when the window has set up
			res = WaitForInputIdle(ShExecInfo.hProcess, 300);

			if (res == WAIT_TIMEOUT) {
				DWORD exitCode = 0;
				//if failed, will set INVALID_HANDLE and return false
				if (GetExitCodeProcess(ShExecInfo.hProcess, &exitCode)) {
					if (exitCode == STILL_ACTIVE) {
						//Remember we are inside WAIT_TIMEOUT, so this means the app is not in idle state yet, so repeat the process until
						//it changes state
						//This can also mean the app is a console app, which never initializes input output loop, so this might need
						// if(CONSOLE_APP){

						// } else{
						// 	continue;
						// }
					} else {
						//this means something has went wrong when loading a dll, or with permissions, or the context of the pp has changed (as we are
						//not executing the app manually)
						// SetLastError(ERROR_INVALID_HANDLE);
						//for now, just return, but we should make a distinction in wether the app failed to load because of an external cause
						//(the user maybe deleted a dll and the program cannot load, but we want to keep executing, or the working directory of the
						//app has changed, as it is being loaded from the root path from deskup, and it fails on setup...)
						return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::Unexpected, 0,
							"WIN_loadProcessFromPath>WaitForInputIdle>GetExitCodeProcess|unexpected_" + path.string()));
					}
				}
			}

			//this means there is a problem with the call itself (for example, an invalid handle), but not a problem of the loading itself
			else if (res == WAIT_FAILED) {
				return std::unexpected(DeskUp::Error::fromLastWinError(GetLastError(), "WIN_loadProcessFromPath>WaitForInputIdle|"));
			}
		}

		//her, the window is successfully idle, and we can safely get the PID from the process handle

		//get the windows PID from the kernel handle
        DWORD pid = GetProcessId(ShExecInfo.hProcess);

		//close the kernel handle, as we already have the pid
        CloseHandle(ShExecInfo.hProcess);

		//this returns nullptr if there it could'nt find the hwnd
        auto hwnd = WIN_FindMainWindow(pid);
        if(!hwnd){
			return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::NotFound, 0, "WIN_loadProcessFromPath>WIN_FindMainWindow|no_hwnd_" + path.string()));
		}

		//set the hwnd inside the device, so that other functions can access this handle
		data->hwnd = hwnd;
	}

	//if nothing went wrong, just return

    return {};
}

DeskUp::Status WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window) noexcept{
	//this function expects to have the hwnd of loadProcessFromPath inside the windowData

    if(!_this || !_this->internalData){
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::DeviceNotFound, 0, "WIN_resizeWindow|no_device"));
    }

    windowData * data = getWindowData(_this);

	//the only way for the hwnd to be nullptr is if in loadProcessFromPath, the function didn't get to the end(at the start is set to nullptr
	//and at the end is set to the valid hwnd), so if the function fails at any point, we can make sure we are not using an invalid hwnd here.
	//Still, if we received an error in loadProcessFromPath, we should treat it before calling this function, or not even calling it at all,
	//so this is just to double check
    if(!data->hwnd){
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::InvalidInput, 0, "WIN_resizeWindow|no_hwnd"));
    }

	auto hwnd = data->hwnd;

    if (window.w <= 0 || window.h <= 0) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "WIN_resizeWindow|invalid_wh"));
    }

	//it might happen that the window closes before accessing it (we don't care the reason), so it is worth checking if the hwnd is valid
	if(!IsWindow(hwnd)){
        return std::unexpected(DeskUp::Error(DeskUp::Level::Skip, DeskUp::ErrType::NotFound, 0, "WIN_resizeWindow|invalid_wh"));
	}

    auto status = retryOp([&] {
        SetLastError(0);

		//get the window size before resizing in order to check if it has changed when resizing
		RECT before;
		GetWindowRect(hwnd, &before);

		LONG beforeW = before.right - before.left;
		LONG beforeH = before.bottom - before.top;

		//if the window is already in the position (which might happen because when executing an app the window has a default (x,y,w,h),
		//and if the user hasn't moved that window it will be reopened in the same position)
		if (before.left == window.x && before.top == window.y && beforeW == window.w && beforeH == window.h){
			return true;
		}

		//returns true if there was no error
		if (SetWindowPos(hwnd, nullptr, window.x, window.y, window.w, window.h, SWP_SHOWWINDOW | SWP_NOZORDER)){
			RECT after;
			GetWindowRect(hwnd, &after);

			LONG afterW = after.right - after.left;
			LONG afterH = after.bottom - after.top;

			//the window has changed, therefore everything was correct
			if (before.left != after.left || before.top != after.top || beforeW != afterW || beforeH != afterH){
				return true;
			}

			// if the position hasn't changed set an error which will be translated into an Level::Info error. It is not necessary to
			// catch it(it can be omitted in the restoreWindows function, the caller of this), but it might be useful later
			SetLastError(ERROR_INVALID_WINDOW_STYLE);
			return false;
		}

		DWORD err = GetLastError();

		//any of privilege errors default to disabled by policy, as we stated in earlier functions, we assume the user has given us all the necessary
		//privileges, and that if the function fails and returns this type of errors it is because the window is protected by windows and we cannot access it.
		if(err == ERROR_ACCESS_DENIED || err == ERROR_PRIVILEGE_NOT_HELD || err == ERROR_ELEVATION_REQUIRED){
			SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);
		}

		//system errors should be fatal and end execution, so just rethrow and let the converter handle it
		else if(err == ERROR_NOT_ENOUGH_MEMORY || err == ERROR_OUTOFMEMORY){
			SetLastError(err);
		}

		else if(err == ERROR_INVALID_WINDOW_HANDLE){
			SetLastError(err);
		}

		//it might happen that in the moment we try resizing the window, the window is unavailable, but it in fact can be moved
		else if(err == ERROR_INVALID_PARAMETER){
			SetLastError(ERROR_FUNCTION_FAILED);
		}

		else{
			SetLastError(err);
		}

		return false;

	}, "WIN_resizeWindow>SetWindowPos|");

	//all of the checking has been done inside the lambda, so just rethrow.
	//Please note that some errors are just info, so in the caller check all the possible errors that can be thrown, or at least do not
	//generalize all the errors to actual errors
    if (!status) {
        return std::unexpected(std::move(status.error()));
    }

    return {};
}

//in here, we don't care the reasons of it failing, if it managed to get the path then it returns true
//this function is used to check against the
static bool WIN_queryPathFromPid(DWORD pid, std::string& out) noexcept{
	//get the process handle (not the pid, this is a direct reference which can be used to alter the process) associated with the pid
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(!h){
		return false;
	}

    DWORD s = 1024;
    std::vector<wchar_t> buf(s, L'\0');
    bool converted = false;
    while(true){
        DWORD size = s;
		//returns the executable inside
        if(QueryFullProcessImageNameW(h, 0, buf.data(), &size)){
            out = WideStringToUTF8(buf.data());
            converted = true;
            break;
        }
        DWORD err = GetLastError();
        if(err == ERROR_INSUFFICIENT_BUFFER){
            s *= 2;
            buf.assign(s, L'\0');
            continue;
        }
        break;
    }
    CloseHandle(h);
    return converted;
}

//returns the pids(PROCESS IDs) of all the processes associated with the path.
//For this, checks all the open processes with a snapshot, gets their associated exe and checks
//if it is the same as the passed. There might be more than one process associated with a single path. note mentioning
static std::vector<DWORD> WIN_getPidsByPath(const fs::path& path) noexcept{
    std::vector<DWORD> pids;
    if(path.empty()){
		return pids;
	}

    const std::string target = normalizePathLower(path.string());

    //this creates a snapshot of all the open processes
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snap == INVALID_HANDLE_VALUE){
		return pids;
	}

    PROCESSENTRY32 pe{};
	pe.dwSize = sizeof(pe);
    if(Process32First(snap, &pe)){
        do{
            DWORD pid = pe.th32ProcessID;
            if(pid == 0){
				continue;
			}

            std::string img;
            if(!WIN_queryPathFromPid(pid, img) || img.empty()){
				continue;
			}

            if(normalizePathLower(img) == target){
                pids.push_back(pid);
            }
        }while(Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return pids;
}

//returns all of the open top-level windows of the process we passed. There might be more than one top level window in the process
static std::vector<HWND> WIN_GetTopLevelWindowsByPid(DWORD pid) noexcept{
    std::vector<HWND> out;

    struct Ctx{
		DWORD pid;
		std::vector<HWND>* out;
	};

    Ctx ctx{pid, &out};

    auto callback = [](HWND hwnd, LPARAM lp)->BOOL{
        Ctx* c = reinterpret_cast<Ctx*>(lp);
        DWORD wpid = 0;
        GetWindowThreadProcessId(hwnd, &wpid);
        if(wpid == c->pid /*the window is part of the current app*/ &&
           GetWindow(hwnd, GW_OWNER) == nullptr /*The window does not have a father (top-level)*/ &&
    	   IsWindowVisible(hwnd) /*The window is visible*/){
            c->out->push_back(hwnd);
        }
        return TRUE;
    };

    EnumWindows(callback, reinterpret_cast<LPARAM>(&ctx));
    return out;
}

//having the pid (PROCESS ID), it closes the process by creating a handle to that same process. ONLY DELETES ONE, THE ONE PASSED
static bool WIN_closeProcessByPid(DWORD pid, DWORD timeoutMs, bool allowForce) noexcept{
    //open a handle to the pid
    HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid);
    if(!h){
		return false;
	}

    auto wins = WIN_GetTopLevelWindowsByPid(pid);
    for(HWND w : wins){
        SendMessageTimeout(w, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 2000, nullptr);
    }

    DWORD wait = WaitForSingleObject(h, timeoutMs);
    if(wait == WAIT_OBJECT_0){
        CloseHandle(h);
        return true;
    }

    if(allowForce){
        TerminateProcess(h, 1);
        WaitForSingleObject(h, 1000);
    }
    CloseHandle(h);

    HANDLE h2 = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if(!h2){
		return true;
	}

    CloseHandle(h2);
    return false;
}

//closes all of the processes associated to a path.
//for this, get all the processes associated with the path (getPidsByPath),
//and close all of them one by one (closeProcessByPid)
static int WIN_closeProcessesByPath(const fs::path& path, DWORD timeoutMs, bool allowForce) noexcept{
    int closed = 0;
    for(DWORD pid : WIN_getPidsByPath(path)){
        if(WIN_closeProcessByPid(pid, timeoutMs, allowForce)){
            closed++;
        }
    }
    return closed;
}

//if there is no currently open window associated with the path, then the function just returns 0, but it doesn't mean it is
//incorrect (mainly because this function gets called without checking if there is another instance of the app), but because there is no
//window to close before opening the app again
DeskUp::Result<unsigned int> WIN_closeProcessFromPath(DeskUpWindowDevice*, const fs::path& path, bool allowForce) noexcept{
    if(path.empty()){
        return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "WIN_closeProcessFromPath|empty_path"));
    }

    int n = WIN_closeProcessesByPath(path, (DWORD) 500, allowForce);
    if(n > 0){
        std::cout << "WIN_closeProcessByPath: Closed " << n << " windows of path: " << path << "\n";
    }

    return n;
}

void WIN_TEST_setHWND(DeskUpWindowDevice* _this, HWND hwnd) {
    if (_this && _this->internalData) {
        reinterpret_cast<windowData*>(_this->internalData)->hwnd = hwnd;
    }
}
