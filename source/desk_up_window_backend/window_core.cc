#include "window_core.h"

std::string DESKUPDIR;

std::unique_ptr<DeskUpWindowDevice> current_window_backend = nullptr;

int DU_Init(){

    const char * devName = winIsAvailable.name;
    if(!winIsAvailable.isAvailable()){
        std::cout << devName << " is not an available backend on this system: Exiting" << std::endl;
        return 0;
    }
    
    DeskUpWindowDevice dev = winWindowDevice.createDevice();

    DESKUPDIR = dev.getDeskUpPath();

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl; 

    current_window_backend = std::make_unique<DeskUpWindowDevice>(dev);
    std::cout << devName << " successfully connected as a backend!" << std::endl;
    
    return 1;
}