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
static DeskUp::Status retryOp(F&& f, std::string_view ctx, int maxAttempts = 3, std::chrono::milliseconds firstDelay = 50ms)
{
    auto delay = firstDelay;
    for (int i = 0; i < maxAttempts; ++i) {
        if (f()){
            return {};
        } 
        DeskUp::Error e = DeskUp::Error::fromLastWinError(ctx, i+1);
        if (e.isFatal()) {
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

DeskUpWindowDevice WIN_CreateDevice(){

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

    device.internalData = (void *) new windowData();
    
    return device;
}

DeskUp::Result<std::string> WIN_getDeskUpPath(){
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


DeskUp::Result<int> WIN_getWindowXPos(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
                                             "WIN_getWindowXPos: invalid HWND"));
    }

    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);
    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; },
                      "WIN_getWindowXPos: GetWindowInfo: ");
    if (!r) return std::unexpected(std::move(r.error()));
    return static_cast<int>(wi.rcWindow.left);
}

DeskUp::Result<int> WIN_getWindowYPos(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
                                             "WIN_getWindowYPos: invalid HWND"));
    }

    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);
    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; },
                      "WIN_getWindowYPos: GetWindowInfo: ");
    if (!r) return std::unexpected(std::move(r.error()));
    return static_cast<int>(wi.rcWindow.top);
}

DeskUp::Result<unsigned int> WIN_getWindowWidth(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
                                             "WIN_getWindowWidth: invalid HWND"));
    }

    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);
    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; },
                      "WIN_getWindowWidth: GetWindowInfo: ");
    if (!r) return std::unexpected(std::move(r.error()));
    return static_cast<unsigned>(wi.rcWindow.right - wi.rcWindow.left);
}

DeskUp::Result<unsigned int> WIN_getWindowHeight(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
                                             "WIN_getWindowHeight: invalid HWND"));
    }

    WINDOWINFO wi{};
    wi.cbSize = sizeof(wi);
    auto r = retryOp([&]{ return GetWindowInfo(data->hwnd, &wi) != 0; },
                      "WIN_getWindowHeight: GetWindowInfo: ");
    if (!r) return std::unexpected(std::move(r.error()));
    return static_cast<unsigned>(wi.rcWindow.bottom - wi.rcWindow.top);
}


DeskUp::Result<std::string> WIN_getPathFromWindow(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !IsWindow(data->hwnd)) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
                                             "WIN_getPathFromWindow: invalid HWND"));
    }

    DWORD pid = 0;

    auto r = retryOp([&]{
        return GetWindowThreadProcessId(data->hwnd, &pid) != 0 && pid != 0;
    }, "WIN_getPathFromWindow: GetWindowThreadProcessId: ");
    if (!r) return std::unexpected(std::move(r.error()));

    HANDLE processHandle = nullptr;
    r = retryOp([&]{
        processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if(!processHandle && GetLastError()==ERROR_ACCESS_DENIED) return true;
        return processHandle != nullptr;
    }, "WIN_getPathFromWindow: OpenProcess: ");
    if (!r) return std::unexpected(std::move(r.error()));
    if (!processHandle) return std::string{};

    std::string result;
    DWORD capacity = 512;
    std::vector<wchar_t> wbuf(capacity);

    for (;;) {
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
                err, "WIN_getPathFromWindow: QueryFullProcessImageNameW: "));
        }
    }

    CloseHandle(processHandle);
    return result;
}


std::string WIN_getNameFromPath(const std::string& path) {
    static int unnamedWindowNum = 0;
    if (path.empty()) {
        return "window" + std::to_string(unnamedWindowNum++);
    }

    try {
        return std::filesystem::path(path).stem().string();
    } catch (...) { return "window" + std::to_string(unnamedWindowNum++); }
}

struct params {
        DeskUpWindowDevice* dev;
        std::vector<windowDesc>* res;
        DeskUp::Error* err;
};

static BOOL CALLBACK WIN_CreateAndSaveWindowProc(HWND hwnd, LPARAM lparam){

    if (!IsWindowVisible(hwnd)) return TRUE;

    RECT r{};
    if (!GetWindowRect(hwnd, &r)) return TRUE;
    if ((r.right - r.left) == 0 || (r.bottom - r.top) == 0) return TRUE;

    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    if (strlen(title) == 0) return TRUE;

    auto* parameters = reinterpret_cast<params*>(lparam);
    if (!parameters || !parameters->res || !parameters->dev || !parameters->dev->internalData) {
        std::cout << "Invalid parameters passed to callback\n";
        return FALSE;
    }

    auto& windows = *parameters->res;
    auto& err     = *parameters->err;
    DeskUpWindowDevice* dev = parameters->dev;

    if (*desk_up_hwnd.get() == hwnd) {
        return TRUE;
    }

    reinterpret_cast<windowData*>(dev->internalData)->hwnd = hwnd;

    windowDesc window;

    if (auto res = WIN_getWindowXPos(dev); res.has_value()) {
        window.x = std::move(res.value());
    } else {
        err = std::move(res.error());
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;
        return err.isFatal() ? FALSE : TRUE;
    }

    if (auto res = WIN_getWindowYPos(dev); res.has_value()) {
        window.y = std::move(res.value());
    } else {
        err = std::move(res.error());
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;
        return err.isFatal() ? FALSE : TRUE;
    }

    if (auto res = WIN_getWindowWidth(dev); res.has_value()) {
        window.w = std::move(res.value());
    } else {
        err = std::move(res.error());
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;
        return err.isFatal() ? FALSE : TRUE;
    }

    if (auto res = WIN_getWindowHeight(dev); res.has_value()) {
        window.h = std::move(res.value());
    } else {
        err = std::move(res.error());
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;
        return err.isFatal() ? FALSE : TRUE;
    }

    if (auto res = WIN_getPathFromWindow(dev); res.has_value()) {
        window.pathToExec = std::move(res.value());
    } else {
        err = std::move(res.error());
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;
        return err.isFatal() ? FALSE : TRUE;
    }

    std::cout << window.pathToExec << std::endl;
    window.name = WIN_getNameFromPath(window.pathToExec);

    reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;

    windows.push_back(std::move(window));
    return TRUE;
}

DeskUp::Result<std::vector<windowDesc>> WIN_getAllOpenWindows(DeskUpWindowDevice* _this){
    std::vector<windowDesc> windows;
    DeskUp::Error error{};


    params p{ _this, &windows, &error };

    HDESK desktop = NULL;

    if (!EnumDesktopWindows(desktop, WIN_CreateAndSaveWindowProc, reinterpret_cast<LPARAM>(&p))) {
        if (error.level() != DeskUp::Level::None) return std::unexpected(std::move(error));
        return std::unexpected(DeskUp::Error::fromLastWinError("WIN_getAllOpenWindows: EnumDesktopWindows"));
    }

    return windows;
}

static inline void stripChars(std::string& s){ 
    if(!s.empty() && s.back() == '\r') 
        s.pop_back(); 
}

DeskUp::Result<windowDesc> WIN_recoverSavedWindow(DeskUpWindowDevice*, std::filesystem::path path) noexcept{

    std::error_code fec;
    if (!fs::exists(path, fec) || !fs::is_regular_file(path, fec)) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
            "WIN_recoverSavedWindow: path is not a regular file: " + path.string()
        ));
    }
    if (fec) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Retry, DeskUp::ErrType::Io, 0,
            "WIN_recoverSavedWindow: filesystem error: " + fec.message()
        ));
    }

    constexpr int kMaxAttempts = 3;
    std::chrono::milliseconds delay{100};
    std::ifstream f;

    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        f.open(path, std::ios::in);
        if (f.is_open()) break;

        if (attempt == kMaxAttempts) {
            return std::unexpected(DeskUp::Error(
                DeskUp::Level::Retry, DeskUp::ErrType::Io, attempt,
                "WIN_recoverSavedWindow: could not open file: " + path.string()
            ));
        }
        std::this_thread::sleep_for(delay);
        delay *= 2;
    }

    windowDesc w = {"", 0, 0, 0, 0, ""};

    std::string s;
    int i = 0;

    auto parse_int = [](const std::string& str, const char*) -> std::optional<int> {
        try {
            size_t pos = 0;
            int val = std::stoi(str, &pos);
            if (pos != str.size()) return std::nullopt;
            return val;
        } catch (...) { return std::nullopt; }
    };

    while (std::getline(f, s)) {
        stripChars(s);
        switch (i) {
            case 0:
                w.pathToExec = s;
                w.name = WIN_getNameFromPath(s);
                break;
            case 1: {
                auto v = parse_int(s, "x");
                if (!v) return std::unexpected(DeskUp::Error(
                    DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0,
                    "WIN_recoverSavedWindow: invalid X coordinate"
                ));
                w.x = *v;
                break;
            }
            case 2: {
                auto v = parse_int(s, "y");
                if (!v) return std::unexpected(DeskUp::Error(
                    DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0,
                    "WIN_recoverSavedWindow: invalid Y coordinate"
                ));
                w.y = *v;
                break;
            }
            case 3: {
                auto v = parse_int(s, "w");
                if (!v || *v < 0) return std::unexpected(DeskUp::Error(
                    DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0,
                    "WIN_recoverSavedWindow: invalid width"
                ));
                w.w = *v;
                break;
            }
            case 4: {
                auto v = parse_int(s, "h");
                if (!v || *v < 0) return std::unexpected(DeskUp::Error(
                    DeskUp::Level::Retry, DeskUp::ErrType::InvalidInput, 0,
                    "WIN_recoverSavedWindow: invalid height"
                ));
                w.h = *v;
                break;
            }
            default:
                break;
        }
        i++;
    }

    if (i < 5) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::InvalidInput, 0,
            "WIN_recoverSavedWindow: file content incomplete (need 5 lines)"
        ));
    }

    return w;
}

//Helper for WIN_loadProcessFromPath. It just returns the specified handle for the pid. It is used to get the hwnd of the launched window
//to resize it later
static HWND WIN_FindMainWindow(DWORD pid, int timeoutMs = 500) {
    HWND hwndFound = nullptr;
    auto enumCallback = [](HWND hwnd, LPARAM lParam) -> BOOL {
        auto data = reinterpret_cast<std::pair<DWORD, HWND*>*>(lParam);
        DWORD winPid;
        GetWindowThreadProcessId(hwnd, &winPid);
        if (winPid == data->first && GetWindow(hwnd, GW_OWNER) == nullptr && IsWindowVisible(hwnd)) {
            *data->second = hwnd;
            return FALSE;
        }
        return TRUE;
    };

    const int step = 100;
    for (int waited = 0; waited < timeoutMs && !hwndFound; waited += step) {
        std::pair<DWORD, HWND*> data{ pid, &hwndFound };
        EnumWindows(enumCallback, reinterpret_cast<LPARAM>(&data));
        if (hwndFound) break;
        Sleep(step);
    }
    return hwndFound;
}

DeskUp::Status WIN_loadProcessFromPath(DeskUpWindowDevice* _this, const std::string path) noexcept {

    if (path.empty()) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal,
            DeskUp::ErrType::InvalidInput,
            0,
            "WIN_loadProcessFromPath: empty path"
        ));
    }

    if (!_this || !_this->internalData) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal,
            DeskUp::ErrType::InvalidInput,
            0,
            "WIN_loadProcessFromPath: invalid window device"
        ));
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

    auto status = retryOp([&] {
        SetLastError(0);
        BOOL ok = ShellExecuteEx(&ShExecInfo);
        DWORD err = GetLastError();
        if (!ok) {
            if (err == 0)
                err = ERROR_FUNCTION_FAILED;
            SetLastError(err);
        }
        return ok != 0;
    }, "WIN_loadProcessFromPath: ShellExecuteEx: ");

    if (!status) {
        return std::unexpected(status.error());
    }

    if (ShExecInfo.hProcess) {
        WaitForInputIdle(ShExecInfo.hProcess, 1500);
        DWORD pid = GetProcessId(ShExecInfo.hProcess);
        CloseHandle(ShExecInfo.hProcess);
        auto hwnd = WIN_FindMainWindow(pid);
        reinterpret_cast<windowData*>(_this->internalData)->hwnd = hwnd;
        if(!hwnd){
            return std::unexpected(DeskUp::Error(DeskUp::Level::Retry, DeskUp::ErrType::NotFound, 0, path));
    }
}

    return {};
}


DeskUp::Status WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window){

    if(!_this || !_this->internalData){
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal,
            DeskUp::ErrType::InvalidInput,
            0,
            "WIN_resizeWindow: invalid window device"
        ));
    }

    windowData * data = reinterpret_cast<windowData*>(_this->internalData);

    if(!data->hwnd){
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Retry,
            DeskUp::ErrType::InvalidInput,
            0,
            "WIN_resizeWindow: hwnd not found"
        ));
    }

    if (window.w <= 0 || window.h <= 0) {
        return std::unexpected(DeskUp::Error(DeskUp::Level::Warning, DeskUp::ErrType::InvalidInput, 0,
            "WIN_resizeWindow: non-positive width/height"));
    }

    auto status = retryOp([&] {
        SetLastError(0);
        BOOL ok = SetWindowPos(data->hwnd, nullptr, window.x, window.y, window.w, window.h,
                        SWP_SHOWWINDOW | SWP_NOZORDER);
        DWORD err = GetLastError();
        if (!ok) {
            if (err == 0)
                err = ERROR_FUNCTION_FAILED;
            SetLastError(err);
        }
        return ok != 0;
    }, "WIN_resizeWindow: SetWindowPos: ");

    if (!status) {
        return std::unexpected(status.error());
    }

    return {};
}

static bool WIN_QueryProcessImagePathA(DWORD pid, std::string& out){
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if(!h) return false;

    DWORD sz = 1024;
    std::vector<wchar_t> buf(sz, L'\0');
    bool converted = false;
    while(1){
        DWORD size = sz;
        if(QueryFullProcessImageNameW(h, 0, buf.data(), &size)){
            out = WideStringToUTF8(buf.data());
            converted = true;
            break;
        }
        DWORD err = GetLastError();
        if(err == ERROR_INSUFFICIENT_BUFFER){
            sz *= 2;
            buf.assign(sz, L'\0');
            continue;
        }
        break;
    }
    CloseHandle(h);
    return converted;
}

static std::vector<DWORD> WIN_getPidsByPath(const std::string& path){
    std::vector<DWORD> pids;
    if(path.empty()) return pids;

    const std::string target = normalizePathLower(path);

    //this creates a snapshot of all the open processes
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snap == INVALID_HANDLE_VALUE) return pids;

    PROCESSENTRY32 pe{}; pe.dwSize = sizeof(pe);
    if(Process32First(snap, &pe)){
        do{
            DWORD pid = pe.th32ProcessID;
            if(pid == 0) continue;

            std::string img;
            if(!WIN_QueryProcessImagePathA(pid, img) || img.empty()) continue;

            if(normalizePathLower(img) == target){
                pids.push_back(pid);
            }
        }while(Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return pids;
}

static std::vector<HWND> WIN_GetTopLevelWindowsByPid(DWORD pid){
    std::vector<HWND> out;

    struct Ctx{ DWORD pid; std::vector<HWND>* out; };
    Ctx ctx{ pid, &out };
    auto cb2 = [](HWND hwnd, LPARAM lp)->BOOL{
        Ctx* c = reinterpret_cast<Ctx*>(lp);
        DWORD wpid = 0;
        GetWindowThreadProcessId(hwnd, &wpid);
        if(wpid == c->pid /*the window is part of the current app*/ &&
            GetWindow(hwnd, GW_OWNER) == nullptr /*The window does not have a father (top-level)*/ && 
            IsWindowVisible(hwnd) /*The window is visible*/)
        {
            c->out->push_back(hwnd);
        }
        return TRUE;
    };
    EnumWindows(cb2, reinterpret_cast<LPARAM>(&ctx));
    return out;
}

static bool WIN_closeProcessByPid(DWORD pid, DWORD timeoutMs, bool allowForce){
    //open a handle to the pid
    HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid);
    if(!h) return false;

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
    if(!h2) return true;
    CloseHandle(h2);
    return false;
}

static int WIN_closeProcessesByPath(const std::string& path, DWORD timeoutMs, bool allowForce){
    int closed = 0;
    for(DWORD pid : WIN_getPidsByPath(path)){
        if(WIN_closeProcessByPid(pid, timeoutMs, allowForce)){
            closed++;
        }
    }
    return closed;
}

DeskUp::Result<unsigned int> WIN_closeProcessFromPath(DeskUpWindowDevice*, const std::string& path, bool allowForce){
    if(path.empty()){
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal,
            DeskUp::ErrType::InvalidInput,
            0,
            "WIN_closeProcessFromPath: empty path"
        ));
    }

    int n = WIN_closeProcessesByPath(path, (DWORD) 1000, allowForce);
    if(n > 0){
        std::cout << "WIN_closeProcessByPath: Closed " << n << " windows of path: " << path << "\n";
    }

    return n;
}