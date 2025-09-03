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

