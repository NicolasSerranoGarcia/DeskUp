#include "window_global.h"

std::string DESKUPDIR;

struct DU_isAvailable{
    const char * name;
    bool (*isAvailable)(void);
};

bool WIN_isAvailable(){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
}


DU_isAvailable win = {
    "win",
    WIN_isAvailable
};

DU_windowDevice * current_window_backend = nullptr;

HWND desk_up_hwnd = nullptr;

//function to initialize the backend and choose the correct device. Previously used x11 and Windows, but now only connects windows
int DU_Init(HWND thisHwnd){

    if(thisHwnd){
        desk_up_hwnd = thisHwnd;
    }

    const char * devName = win.name;
    if(!win.isAvailable()){
        std::cout << devName << " is not an available backend on this system: Exiting" << std::endl;
        return 0;
    }
    
    DU_windowDevice * dev = winWindowDevice.createDevice();

    if(dev == nullptr){
        return 0;
    }

    DESKUPDIR = dev->getDeskUpPath();

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl; 

    current_window_backend = dev;
    std::cout << devName << " successfully connected as a backend!" << std::endl;
    
    return 1;
}