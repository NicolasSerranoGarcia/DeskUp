#include "desk_up_win.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include <filesystem>

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

// For now, the app considers only the first DeskUp window
BOOL CALLBACK WIN_isDeskUp(HWND hwnd, LPARAM lparam){
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    DWORD myPid = GetCurrentProcessId();
    if (pid == myPid) {
        HWND * h = reinterpret_cast<HWND*>(lparam);
        *h = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND WIN_getDeskUpHWND(){

    HWND myWindows;

    // For now, default desktop
    HDESK desk = NULL;
    EnumDesktopWindows(desk, WIN_isDeskUp, reinterpret_cast<LPARAM>(&myWindows));

    return myWindows;
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
    
    const unsigned int x = pwi.rcWindow.left;
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
    
    const unsigned int y = pwi.rcWindow.top;
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
    
    const unsigned int width  = pwi.rcWindow.right  - pwi.rcWindow.left;
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
    
    const unsigned int height = pwi.rcWindow.bottom - pwi.rcWindow.top;
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

BOOL CALLBACK WIN_createAndSaveWindow(HWND hwnd, LPARAM lparam){
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
}

std::vector<windowDesc> WIN_getAllOpenWindows(DeskUpWindowDevice * _this){
    
    std::vector<windowDesc> windows;

    std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> callbackParameters{&windows, _this};
    
    HDESK desktop = NULL;

    // Pass vector + device through LPARAM
    if(!EnumDesktopWindows(desktop, /*callback*/ WIN_createAndSaveWindow, reinterpret_cast<LPARAM>((void *) &callbackParameters))){
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

void WIN_loadProcessFromPath(DeskUpWindowDevice * _this, std::string path){

    if(path.empty()){
        throw std::invalid_argument("WIN_loadProcessFromPath: the path is empty!");
    }

    if(!_this || !_this->internalData){
        throw std::invalid_argument("WIN_loadProcessFromPath: the device!");
    }

    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = 0;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = (TCHAR *) path.c_str();
    ShExecInfo.lpParameters = NULL;
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_NORMAL;
    ShExecInfo.hInstApp = NULL;

    if(!ShellExecuteEx(&ShExecInfo)){
        throw new std::runtime_error(getSystemErrorMessageWindows(GetLastError(), "WIN_loadProcessFromPath: "));
    }

    ((windowData *)_this->internalData)->hwnd = ShExecInfo.hwnd;
}

void WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window){
    
}