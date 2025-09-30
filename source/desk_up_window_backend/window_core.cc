#include "window_core.h"

std::string DESKUPDIR;

std::unique_ptr<DeskUpWindowDevice> current_window_backend = nullptr;

DeskUpWindowBootStrap devices[] = { winWindowDevice };

int numDevices = 1;

int DU_Init(){

    DeskUpWindowDevice dev;
    
    std::string devName;

    bool assigned = false;
    for(int i = 0; i < numDevices; i++){
        
        if(!devices[i].isAvailable()){
            std::cout << devices[i].name << " is not an available backend on this system: Exiting" << std::endl;
            continue;
        }

        devName = devices[i].name;
        dev = devices[i].createDevice();
        assigned = true;
        break;
    }

    if(!assigned){
        return 0;
    }

    DESKUPDIR = dev.getDeskUpPath();

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl; 

    current_window_backend = std::make_unique<DeskUpWindowDevice>(dev);
    std::cout << devName << " successfully connected as a backend!" << std::endl;
    
    return 1;
}