#include "desk_up_win.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include <filesystem>
#include <tlhelp32.h> 

#include "backend_utils.h"

namespace fs = std::filesystem;


static std::unique_ptr<HWND> desk_up_hwnd = nullptr;

struct windowData{
    HWND hwnd;
};

DeskUpWindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice,
    WIN_isAvailable
};

bool WIN_isAvailable(){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
}

static HWND WIN_getDeskUpHWND(){

    HWND deskUpWindow;

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
    device.getAllOpenWindows   = WIN_getAllOpenWindows;
    device.getDeskUpPath   = WIN_getDeskUpPath;
    device.loadWindowFromPath = WIN_loadProcessFromPath;
    device.recoverSavedWindow = WIN_recoverSavedWindow;
    device.resizeWindow    = WIN_resizeWindow;
    device.closeWindowFromPath = WIN_closeProcessFromPath;

    device.internalData = (void *) new windowData();
    
    return device;
}

std::string WIN_getDeskUpPath(){
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

int WIN_getWindowXPos(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("WIN_getWindowXPos: invalid HWND");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    // Fill pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN_getWindowXPos: GetWindowInfo: ";
        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);
        delete (windowData *) _this->internalData;
        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int x = pwi.rcClient.left;
    return x;
}

int WIN_getWindowYPos(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("WIN_getWindowYPos: invalid HWND");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    // Fill pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN_getWindowYPos: GetWindowInfo: ";
        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);
        delete (windowData *) _this->internalData;
        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int y = pwi.rcClient.top;
    return y;
}

unsigned int WIN_getWindowWidth(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("WIN_getWindowWidth: invalid HWND");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    // Fill pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN_getWindowWidth: GetWindowInfo: ";
        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);
        delete (windowData *) _this->internalData;
        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int width  = pwi.rcClient.right  - pwi.rcClient.left;
    return width;
}

unsigned int WIN_getWindowHeight(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("WIN_getWindowHeight: invalid HWND");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    // Fill pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN_getWindowHeight: GetWindowInfo: ";
        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);
        delete (windowData *) _this->internalData;
        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int height = pwi.rcClient.bottom - pwi.rcClient.top;
    return height;
}

std::string WIN_getPathFromWindow(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !data->hwnd || !IsWindow(data->hwnd)) {
        throw std::invalid_argument("WIN_getPathFromWindow: invalid HWND");
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(data->hwnd, &pid) || pid == 0) {
        const DWORD err = GetLastError();
        delete (windowData *) _this->internalData;
        throw std::runtime_error(getSystemErrorMessageWindows(err, "WIN_getPathFromWindow: GetWindowThreadProcessId: "));
    }

    HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!processHandle) {
        const DWORD err = GetLastError();
        if(err == ERROR_ACCESS_DENIED){
            return "";
        }
        delete (windowData *) _this->internalData;
        throw std::runtime_error(getSystemErrorMessageWindows(err, "WIN_getPathFromWindow: OpenProcess: "));
    }

    std::string result;
    try {

        DWORD dwSize = 512;
        std::vector<wchar_t> wbuf(dwSize);

        while(1){
            DWORD size = dwSize;
            if (QueryFullProcessImageNameW(processHandle, 0, wbuf.data(), &size)) {
                result = WideStringToUTF8(wbuf.data());
                break;
            }
            DWORD err = GetLastError();
            if (err == ERROR_INSUFFICIENT_BUFFER) {
                dwSize *= 2;
                wbuf.assign(dwSize, L'\0');
                continue;
            }
            throw std::runtime_error(getSystemErrorMessageWindows(err, "WIN_getPathFromWindow: QueryFullProcessImageNameW: "));
        }
    } catch (...) {
        delete (windowData *) _this->internalData;
        CloseHandle(processHandle);
        throw;
    }

    CloseHandle(processHandle);
    return result;
}

std::string WIN_getNameFromPath(const std::string& path) {
    static int unnamedWindowNum = 0;
    if (path.empty()) {
        std::cerr << "path is empty\n";
        std::string p = "window";
        p+= std::to_string(unnamedWindowNum);
        unnamedWindowNum++;
        return p;
    }

    std::string name;
    for (int i = (int)path.length() - 1; i >= 0; --i) {
        if (path[i] == '\\' || path[i] == '/')
            break;
        name.insert(name.begin(), path[i]);
    }

    return name.substr(0, name.size() - 4);
}

std::vector<windowDesc> WIN_getAllOpenWindows(DeskUpWindowDevice * _this){

    auto createAndSaveWindow = [](HWND hwnd, LPARAM lparam){
        if (!IsWindowVisible(hwnd)) return TRUE;
        RECT r;
        if (!GetWindowRect(hwnd, &r)) return TRUE;
        if ((r.right - r.left) == 0 || (r.bottom - r.top) == 0) return TRUE;

        // Retrieve parameters and types
        std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> * parameters = (std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> *)
                                                                                                                reinterpret_cast<void *>(lparam);

        std::vector<windowDesc> * windows = parameters->first;
        DeskUpWindowDevice * dev = parameters->second;

        if(windows == nullptr){
            std::cout << "The vector of windows could not be passed to the callback!" << std::endl;
            return FALSE;
        }

        if(dev == nullptr){
            std::cout << "The device could not be passed to the callback!" << std::endl;
            return FALSE;
        }

        if(dev->internalData == nullptr){
            std::cout << "The device could not be passed to the callback!" << std::endl;
            return FALSE;
        }

        // Omit DeskUp window
        if(*desk_up_hwnd.get() == hwnd){
            return TRUE;
        }

        // Build and add windowDesc
        windowDesc window;
        
        try{
            // Load HWND into the device so backend functions use it
            reinterpret_cast<windowData*>(dev->internalData)->hwnd = hwnd;

            window.x = WIN_getWindowXPos(dev);
            window.y = WIN_getWindowYPos(dev);
            window.h = WIN_getWindowHeight(dev);
            window.w = WIN_getWindowWidth(dev);
            
            std::cout << ": " << window.x << ", " << window.y;

            window.pathToExec = WIN_getPathFromWindow(dev);
            
            std::cout << window.pathToExec << std::endl;
            window.name = WIN_getNameFromPath(window.pathToExec);

            // Clear to avoid accidentally reusing HWND
            reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;

        } catch(std::invalid_argument &error){
            std::cout << "WIN_createAndSaveWindow: invalid argument: " << error.what() << std::endl;
            SetLastError(ERROR_BAD_ARGUMENTS);
            return FALSE;
        } catch(std::runtime_error &error){
            std::cout << "WIN_createAndSaveWindow: windows error: " << error.what() << std::endl;
            SetLastError(ERROR_BAD_COMMAND);
            return FALSE;
        } catch(...){
            std::cout << "WIN_createAndSaveWindow: unexpected error" << std::endl;
            SetLastError(ERROR_FUNCTION_FAILED);
            return FALSE;
        }

        windows->push_back(window);

        return TRUE;
    };

    std::vector<windowDesc> windows;

    std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> callbackParameters{&windows, _this};
    
    HDESK desktop = NULL;

    // Pass vector + device through LPARAM
    if(!EnumDesktopWindows(desktop, /*callback*/ createAndSaveWindow, reinterpret_cast<LPARAM>((void *) &callbackParameters))){
        DWORD error = GetLastError();
        std::string errorMessage = getSystemErrorMessageWindows(error, "WIN_getAllOpenWindows: ");
        delete (windowData *) _this->internalData;
        throw std::runtime_error(errorMessage);
    }

    return windows;
}

windowDesc WIN_recoverSavedWindow(DeskUpWindowDevice *, std::filesystem::path path){

    std::ifstream f;

    f.open(path.string(), std::ios::in);
    
    
    if(!f.is_open()){
        throw std::runtime_error("WIN_recoverSavedWindow: Could not open the file containing the window!");
    }

    windowDesc w = {"",0,0,0,0,""};
    std::string s;
    int i = 0;

    while(std::getline(f, s)){
        switch(i){
            case 0:
                w.pathToExec = s;
                w.name = WIN_getNameFromPath(s);
                break;
            case 1:
                w.x = std::stoi(s);
                break;
            case 2: 
                w.y = std::stoi(s);
                break;
            case 3:
                w.w = std::stoi(s);
                break;
            case 4:
                w.h = std::stoi(s);
                break;
            default:
                break;
        }
        i++;
    }

    return w;
}

//Helper for WIN_loadProcessFromPath. It just returns the specified handle for the pid. It is used to get the hwnd of the launched window
//to resize it later
static HWND WIN_FindMainWindow(DWORD pid, int timeoutMs = 5000) {
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

void WIN_loadProcessFromPath(DeskUpWindowDevice * _this, const std::string path){

    if(path.empty()){
        throw std::invalid_argument("WIN_loadProcessFromPath: the path is empty!");
    }

    if(!_this || !_this->internalData){
        throw std::invalid_argument("WIN_loadProcessFromPath: there was an error parsing the window device!");
    }

    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOASYNC;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = nullptr;
    ShExecInfo.lpFile = path.c_str();
    ShExecInfo.lpParameters = nullptr;
    ShExecInfo.lpDirectory = nullptr;
    ShExecInfo.nShow = SW_NORMAL;
    ShExecInfo.hInstApp = NULL;

    if(!ShellExecuteEx(&ShExecInfo)){
        throw std::runtime_error(getSystemErrorMessageWindows(GetLastError(), "WIN_loadProcessFromPath: "));
    }


    if (ShExecInfo.hProcess) {
        WaitForInputIdle(ShExecInfo.hProcess, 2000);
        DWORD pid = GetProcessId(ShExecInfo.hProcess);

        
        CloseHandle(ShExecInfo.hProcess);
        //FindMainWindow can return nullptr, but it will be handled later
        ((windowData *)_this->internalData)->hwnd = WIN_FindMainWindow(pid);
    }
}

void WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window){

    if(!_this || !_this->internalData){
        throw std::invalid_argument("WIN_resizeWindow: there was an error parsing the window device!");
    }

    windowData * data = reinterpret_cast<windowData*>(_this->internalData);

    if(!data->hwnd){
        //this should not block execution. 
        //throw std::runtime_error("WIN_resizeWindow: could not recover the handle to the window correctly!");
        //for the moment just return
        std::cout << "WIN_resizeWindow: no HWND for '" << window.name 
                    << "' (" << window.pathToExec << "), skipping resize.\n";
        return;
    }

    if(!SetWindowPos(data->hwnd, nullptr, window.x, window.y, window.w, window.h, SWP_SHOWWINDOW)){
        throw std::runtime_error(getSystemErrorMessageWindows(GetLastError(), "WIN_resizeWindow: "));
    }
}

//Next is helpers for WIN_relaunchAndResize()
static bool QueryProcessImagePathA(DWORD pid, std::string& out){
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
            if(!QueryProcessImagePathA(pid, img) || img.empty()) continue;

            if(normalizePathLower(img) == target){
                pids.push_back(pid);
            }
        }while(Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return pids;
}

static bool WIN_isProcessOpenByPath(const std::string& path){
    auto v = WIN_getPidsByPath(path);
    return !v.empty();
}

static std::vector<HWND> GetTopLevelWindowsByPid(DWORD pid){
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

    auto wins = GetTopLevelWindowsByPid(pid);
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

unsigned int WIN_closeProcessFromPath(DeskUpWindowDevice*, const std::string& path, bool allowForce){
    if(path.empty()){
        throw std::invalid_argument("WIN_relaunchAndResize: the path to the executable is empty!");
    }

    int n = WIN_closeProcessesByPath(path, (DWORD) 1000, allowForce);
    if(n > 0){
        std::cout << "WIN_relaunchAndResize: Closed " << n << "windows of path: " << path << "\n";
    }

    return n;
}