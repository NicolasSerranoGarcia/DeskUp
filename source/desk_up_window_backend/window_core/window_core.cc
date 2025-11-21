#include "window_core.h"

#include <vector>

#ifdef _WIN32
    #include "desk_up_win.h"
#endif

std::vector<DeskUpWindowBootStrap> devices;

std::string DESKUPDIR;

std::unique_ptr<DeskUpWindowDevice> current_window_backend = nullptr;

int DU_Init(){

    #ifdef _WIN32
        devices.push_back(winWindowDevice);
    #endif

    DeskUpWindowDevice dev;

    std::string devName;

    bool assigned = false;
    for(unsigned int i = 0; i < devices.size(); i++){

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

    if(auto res = dev.getDeskUpPath(); res.has_value()){
        DESKUPDIR = std::move(res.value());
    } else{
        return 0;
    }

    std::cout << "DeskUp path: " << DESKUPDIR << std::endl;

    current_window_backend = std::make_unique<DeskUpWindowDevice>(dev);
    std::cout << devName << " successfully connected as a backend!" << std::endl;

    return 1;
}

void DU_Destroy(){
	current_window_backend.get()->DestroyDevice(current_window_backend.get());
    current_window_backend.reset();
    DESKUPDIR.clear();
    devices.clear();
}