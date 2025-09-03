#include "desk_up_win.h"

#include "backend_utils.h"

bool WIN_isAvailable(DU_WindowDevice *){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
}

DU_WindowDevice * WIN_CreateDevice(void){
    //set all the functions of a DU_windowDevice variable to the functions of x11. Also set internalData to 

    DU_WindowDevice * device = nullptr;

    device->getWindowHeight = WIN_getWindowHeight;
    device->isAvailable = WIN_isAvailable;

    windowData * data;

    device->internalData = data;
    
    return device;
}

unsigned int WIN_getWindowHeight(DU_WindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments!");
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

unsigned int WIN_getWindowWidth(DU_WindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments!");
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

unsigned int WIN_getWindowXPos(DU_WindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments!");
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

unsigned int WIN_getWindowYPos(DU_WindowDevice * _this){
    const windowData * data = (windowData *) _this->internalData;
    
    if(!data->hwnd){
        throw std::invalid_argument("Invalid arguments!");
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

std::filesystem::path WIN_GetPathFromWindow(DU_WindowDevice * _this){

}

BOOL CALLBACK WIN_createAndSaveWindow(HWND hwnd, LPARAM lparam){

    //recover the parameters once we are inside. We can now use both DU_WindowDevice and fill the vector with windows
    std::pair<std::vector<windowDesc> *, DU_WindowDevice *> * parameters = (std::pair<std::vector<windowDesc> *, DU_WindowDevice *> *) reinterpret_cast<void *>(lparam);

    std::vector<windowDesc> * windows = parameters->first;
    DU_WindowDevice * dev = parameters->second;

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
        
        window.pathToExec = WIN_GetPathFromWindow(dev);

        //after that erase it so that we dont use a previous hwnd by accident
        ((windowData *) dev->internalData)->hwnd = {0};

    } catch(std::invalid_argument error){
        std::cout << "An error occurred when passing arguments to the functions that return window attributes:" << error.what() << std::endl;
    } catch(std::runtime_error error){
        std::cout << "A windows error occurred when trying to access the window properties:" << error.what() << std::endl;
    } catch(...){
        std::cout << "something unexpected happened" << std::endl;
    }


    windows->push_back(window);

    return TRUE;
}

std::vector<windowDesc> WIN_getAllWindows(DU_WindowDevice * _this){
    
    std::vector<windowDesc> windows;

    std::pair<std::vector<windowDesc> *, DU_WindowDevice *> * callbackParameters;

    callbackParameters->first = &windows;
    callbackParameters->second = _this;
    
    //we need to "fit" both the vector of the windows and the own DU_WindowDevice inside the callback inside LPARAM
    EnumDesktopWindows(NULL, WIN_createAndSaveWindow, reinterpret_cast<LPARAM>((void *) &callbackParameters));
}


