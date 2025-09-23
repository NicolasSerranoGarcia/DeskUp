#include "desk_up_win.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <shlobj.h>
#include <filesystem>

#include "backend_utils.h"

std::unique_ptr<HWND> desk_up_hwnd = nullptr;

struct windowData{
    HWND hwnd;
};

DeskUpWindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice
};

DeskUpisAvailable winIsAvailable = {
    "win",
    WIN_isAvailable
};

bool WIN_isAvailable(){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
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

unsigned int WIN_getWindowHeight(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowHeight!");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
        
    }
    
    const unsigned int height = pwi.rcWindow.bottom - pwi.rcWindow.top;
    return height;
}

unsigned int WIN_getWindowWidth(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowWidth!");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
        
    }
    
    const unsigned int width  = pwi.rcWindow.right  - pwi.rcWindow.left;
    return width;
}

int WIN_getWindowXPos(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowXPos!");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int x = pwi.rcWindow.left;
    return x;
}

int WIN_getWindowYPos(DeskUpWindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowYPos!");
    }

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, &pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        std::string errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int y = pwi.rcWindow.top;
    return y;
}

//every os works with different types for interpreting paths, so work with std 
std::string WIN_GetPathFromWindow(DeskUpWindowDevice* _this) {
    const auto* data = static_cast<const windowData*>(_this->internalData);
    if (!data || !data->hwnd || !IsWindow(data->hwnd)) {
        throw std::invalid_argument("Invalid HWND in WIN_GetPathFromWindow");
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(data->hwnd, &pid) || pid == 0) {
        const DWORD err = GetLastError();
        throw std::runtime_error(getSystemErrorMessageWindows(err, "GetWindowThreadProcessId: "));
    }

    HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!processHandle) {
        const DWORD err = GetLastError();
        if(err == ERROR_ACCESS_DENIED){
            return "";
        }
        throw std::runtime_error(getSystemErrorMessageWindows(err, "OpenProcess: "));
    }

    std::string result;
    try {

        DWORD dwSize = 512;
        std::vector<wchar_t> wbuf(dwSize);

        for (;;) {
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
            throw std::runtime_error(getSystemErrorMessageWindows(err, "QueryFullProcessImageNameW: "));
        }
    } catch (...) {
        CloseHandle(processHandle);
        throw;
    }

    CloseHandle(processHandle);
    return result;
}

std::string WIN_GetNameFromPath(const std::string& path) {
    if (path.empty()) {
        std::cerr << "path is empty\n";
        return "";
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

    //recover the parameters once we are inside. We can now use both DU_windowDevice and fill the vector with windows
    std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> * parameters = (std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> *) reinterpret_cast<void *>(lparam);

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

    //skip deskUp
    if(*desk_up_hwnd.get() == hwnd){
        return TRUE;
    }

    //fill a new windowDesc and push it to 'windows'

    windowDesc window;
    
    try{
        //load the internal device with the identifier of the window so that backend functions can use them
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = hwnd;

        window.x = WIN_getWindowXPos(dev);
        window.y = WIN_getWindowYPos(dev);
        window.h = WIN_getWindowHeight(dev);
        window.w = WIN_getWindowWidth(dev);
        
        std::cout << ": " << window.x << ", " << window.y;

        window.pathToExec = WIN_GetPathFromWindow(dev);
        
        std::cout << window.pathToExec << std::endl;
        window.name = WIN_GetNameFromPath(window.pathToExec);

        //after that erase it so that we dont use a previous hwnd by accident
        reinterpret_cast<windowData*>(dev->internalData)->hwnd = nullptr;

    } catch(std::invalid_argument &error){
        std::cout << "An error occurred when passing arguments to the functions that return window attributes:" << error.what() << std::endl;
        SetLastError(ERROR_BAD_ARGUMENTS);
        return FALSE;
    } catch(std::runtime_error &error){
        std::cout << "A windows error occurred when trying to access the window properties:" << error.what() << std::endl;
        SetLastError(ERROR_BAD_COMMAND);
        return FALSE;
    } catch(...){
        std::cout << "something unexpected happened" << std::endl;
        SetLastError(ERROR_FUNCTION_FAILED);
        return FALSE;
    }


    windows->push_back(window);

    return TRUE;
}

std::vector<windowDesc> WIN_getAllWindows(DeskUpWindowDevice * _this){
    
    std::vector<windowDesc> windows;

    std::pair<std::vector<windowDesc> *, DeskUpWindowDevice *> callbackParameters{&windows, _this};
    
    HDESK desktop = NULL;

    //we need to "fit" both the vector of the windows and the own DeskUpWindowDevice inside the callback inside LPARAM
    if(!EnumDesktopWindows(desktop, /*callback*/ WIN_createAndSaveWindow, reinterpret_cast<LPARAM>((void *) &callbackParameters))){
        DWORD error = GetLastError();
        std::string errorMessage = getSystemErrorMessageWindows(error);
        
        throw std::runtime_error(errorMessage);
    }

    return windows;
}


DeskUpWindowDevice * WIN_CreateDevice(HWND deskUpHWND){
    //set all the functions of a DeskUpWindowDevice variable to the functions of x11. Also set internalData to 

    if(deskUpHWND){
        desk_up_hwnd = std::make_unique<HWND>(deskUpHWND);
    }

    DeskUpWindowDevice * device;

    try{
        device = new DeskUpWindowDevice;
    } catch(std::bad_alloc &a){
        std::cerr << a.what();
        return nullptr;
    }

    device->getWindowHeight = WIN_getWindowHeight;
    device->getWindowWidth = WIN_getWindowWidth;
    device->getWindowXPos = WIN_getWindowXPos;
    device->getWindowYPos = WIN_getWindowYPos;
    device->getAllWindows = WIN_getAllWindows;
    device->getDeskUpPath = WIN_getDeskUpPath;

    device->internalData = (void *) new windowData();

    
    
    return device;
}