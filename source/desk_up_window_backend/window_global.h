#ifndef WINDOWGLOBAL_H
#define WINDOWGLOBAL_H

#include <iostream>
#include <vector>
#include "windowDesc.h"
#include "desk_up_win.h"

const char * DESKUPDIR = "";

typedef struct DU_windowDevice{
    //here goes all the generic functions that all the backends have
    unsigned int (*getWindowHeight)(DU_windowDevice * _this);
    unsigned int (*getWindowWidth)(DU_windowDevice * _this);
    unsigned int (*getWindowXPos)(DU_windowDevice * _this);
    unsigned int (*getWindowYPos)(DU_windowDevice * _this);
    
    std::vector<windowDesc> (*getAllWindows)(DU_windowDevice * _this);

    void * internalData;
};

typedef struct DU_isAvailable{
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

typedef struct DU_WindowBootStrap{
    const char * name;
    DU_windowDevice *(*createDevice)(void);
};

DU_windowDevice * current_window_backend;

extern DU_WindowBootStrap winWindowDevice;

//function to initialize the backend and choose the correct device. Previously used x11 and Windows, but now only connects windows
int DU_Init(){
        
    const char * devName = win.name;
    if(!win.isAvailable()){
        std::cout << devName << "is not an available backend on this system: Exiting" << std::endl;
        return 0;
    }
    
    DU_windowDevice * dev = winWindowDevice.createDevice();

    current_window_backend = dev;
    std::cout << devName << "successfully connected as a backend!" << std::endl;
    
    return 1;
}


#endif