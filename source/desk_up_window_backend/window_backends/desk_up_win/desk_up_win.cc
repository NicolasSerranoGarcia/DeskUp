#include "desk_up_win.h"

#include "backend_utils.h"

bool WIN_isAvailable(DU_windowDevice *){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
}

DU_windowDevice * WIN_CreateDevice(void){
    //set all the functions of a DU_windowDevice variable to the functions of x11. Also set internalData to 

    DU_windowDevice * device = nullptr;

    device->getWindowHeight = WIN_getWindowHeight;
    device->isAvailable = WIN_isAvailable;

    windowData * data;

    device->internalData = data;
    
    return device;
}

unsigned int WIN_getWindowHeight(DU_windowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowHeight!");
    }

    PWINDOWINFO pwi;
    pwi->cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        const char * errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
        
    }
    
    const unsigned int height = pwi->cyWindowBorders;
    return height;
}

unsigned int WIN_getWindowWidth(DU_windowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowWidth!");
    }

    PWINDOWINFO pwi;
    pwi->cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        const char * errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
        
    }
    
    const unsigned int width = pwi->cxWindowBorders;
    return width;
}

unsigned int WIN_getWindowXPos(DU_windowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowXPos!");
    }

    PWINDOWINFO pwi;
    pwi->cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        const char * errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int x = pwi->rcWindow.left;
    return x;
}

unsigned int WIN_getWindowYPos(DU_windowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetWindowYPos!");
    }

    PWINDOWINFO pwi;
    pwi->cbSize = sizeof(WINDOWINFO);
    
    //fills pwi with information about the given window
    if(!GetWindowInfo(data->hwnd, pwi)){
        DWORD windowsError = GetLastError();
        const char * contextMessage = "WIN32 could not return the window height! Cause:";

        const char * errorMessage = getSystemErrorMessageWindows(windowsError, contextMessage);

        throw std::runtime_error(errorMessage);
    }
    
    const unsigned int y = pwi->rcWindow.top;
    return y;
}

//every os works with different types for interpreting paths, so work with std 
char * WIN_GetPathFromWindow(DU_windowDevice * _this){

    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments in function WIN_GetPathFromWindow!");
    }

    DWORD pid;


    if(!GetWindowThreadProcessId(data->hwnd, &pid)){
        DWORD error = GetLastError();
        const char * errorMessage = getSystemErrorMessageWindows(error, "GetWindowProcessId: ");
        throw std::runtime_error(errorMessage);
    }

    //this reopens the process handle that created the window
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, pid);

    if(!processHandle){
        DWORD windowsError = GetLastError();
        const char * errorMessage = getSystemErrorMessageWindows(windowsError, "OpenProcess: ");

        throw std::runtime_error(errorMessage);
    }

    LPWSTR path;
    DWORD dwSize = 500;
    LPDWORD lpdwSize = &dwSize;

    //this specifies the path format. '0' means win32 file format. In short, this is the 
    //path that has the letters (C:\Users) in the path
    DWORD dwflags = 0x0;

    if(!QueryFullProcessImageNameW(processHandle, dwflags, path, lpdwSize)){
        DWORD error = GetLastError();
        const char * errorMessage = getSystemErrorMessageWindows(error, "QueryFullProcessImageNameW: ");

        throw std::runtime_error(errorMessage);
    }

    //convert from the type of 'path' to std::filesystem
    const int maxLength = 500;
    
    char buffer[maxLength];


    wcstombs(buffer, path, maxLength);

    //buffer has the path
    return buffer;
}

BOOL CALLBACK WIN_createAndSaveWindow(HWND hwnd, LPARAM lparam){

    //recover the parameters once we are inside. We can now use both DU_windowDevice and fill the vector with windows
    std::pair<std::vector<windowDesc> *, DU_windowDevice *> * parameters = (std::pair<std::vector<windowDesc> *, DU_windowDevice *> *) reinterpret_cast<void *>(lparam);

    std::vector<windowDesc> * windows = parameters->first;
    DU_windowDevice * dev = parameters->second;

    if(windows == nullptr){
        std::cout << "The vector of windows could not be passed to the callback!" << std::endl;
        return FALSE;
    }

    //fill a new windowDesc and push it to 'windows'

    windowDesc window;

    try{
        //load the internal device with the identifier of the window
        ((windowData *) dev->internalData)->hwnd = hwnd;

        window.x = WIN_getWindowXPos(dev);
        window.y = WIN_getWindowYPos(dev);
        window.h = WIN_getWindowHeight(dev);
        window.w = WIN_getWindowWidth(dev);
        
        window.pathToExec = std::string(WIN_GetPathFromWindow(dev));

        //after that erase it so that we dont use a previous hwnd by accident
        ((windowData *) dev->internalData)->hwnd = {0};

    } catch(std::invalid_argument error){
        std::cout << "An error occurred when passing arguments to the functions that return window attributes:" << error.what() << std::endl;
        SetLastError(ERROR_BAD_ARGUMENTS);
        return FALSE;
    } catch(std::runtime_error error){
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

std::vector<windowDesc> WIN_getAllWindows(DU_windowDevice * _this){
    
    std::vector<windowDesc> windows;

    std::pair<std::vector<windowDesc> *, DU_windowDevice *> * callbackParameters;

    callbackParameters->first = &windows;
    callbackParameters->second = _this;
    
    HDESK desktop = NULL;

    //we need to "fit" both the vector of the windows and the own DU_windowDevice inside the callback inside LPARAM
    if(!EnumDesktopWindows(desktop, /*callback*/ WIN_createAndSaveWindow, reinterpret_cast<LPARAM>((void *) &callbackParameters))){
        DWORD error = GetLastError();
        const char * errorMessage = getSystemErrorMessageWindows(error);
        
        throw std::runtime_error(errorMessage);
    }

    return windows;
}


