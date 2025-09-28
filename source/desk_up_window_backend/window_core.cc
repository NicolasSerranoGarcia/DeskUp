#include "window_core.h"

std::string DESKUPDIR;

DeskUpWindowDevice * current_window_backend = nullptr;

//function to initialize the backend and choose the correct device. Previously used x11 and Windows, but now only connects windows
int DU_Init(){

    const char * devName = winIsAvailable.name;
    if(!winIsAvailable.isAvailable()){
        std::cout << devName << " is not an available backend on this system: Exiting" << std::endl;
        return 0;
    }
    
    DeskUpWindowDevice * dev = winWindowDevice.createDevice();

    if(dev == nullptr){
        return 0;
    }

    DESKUPDIR = dev->getDeskUpPath();

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl; 

    current_window_backend = dev;
    std::cout << devName << " successfully connected as a backend!" << std::endl;
    
    return 1;
}