#include "window_core.h"

std::string DESKUPDIR;

struct DeskUpisAvailable{
    const char * name;
    bool (*isAvailable)(void);
};

bool WIN_isAvailable(){
    #ifdef _WIN32
        return true;
    #endif
    
    return false;
}

//add this in a separate file and create the instance inside desk_up_win
DeskUpisAvailable win = {
    "win",
    WIN_isAvailable
};

DeskUpWindowDevice * current_window_backend = nullptr;


//function to initialize the backend and choose the correct device. Previously used x11 and Windows, but now only connects windows
int DU_Init(HWND thisHwnd){

    const char * devName = win.name;
    if(!win.isAvailable()){
        std::cout << devName << " is not an available backend on this system: Exiting" << std::endl;
        return 0;
    }
    
    HWND h = nullptr;
    if(thisHwnd){
        h = thisHwnd;
    }
    DeskUpWindowDevice * dev = winWindowDevice.createDevice(h);

    if(dev == nullptr){
        return 0;
    }

    DESKUPDIR = dev->getDeskUpPath();

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl; 

    current_window_backend = dev;
    std::cout << devName << " successfully connected as a backend!" << std::endl;
    
    return 1;
}