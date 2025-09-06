#ifndef WINDOWGLOBAL_H
#define WINDOWGLOBAL_H

#include <iostream>
#include <vector>
#include "windowDesc.h"

typedef struct DU_windowDevice{
    //here goes all the generic functions that all the backends have
    unsigned int (*getWindowHeight)(DU_windowDevice * _this);
    unsigned int (*getWindowWidth)(DU_windowDevice * _this);
    unsigned int (*getWindowXPos)(DU_windowDevice * _this);
    unsigned int (*getWindowYPos)(DU_windowDevice * _this);
    
    std::vector<windowDesc> (*getAllWindows)(void);

    bool (*isAvailable)(DU_windowDevice * _this);

    void * internalData;
};

typedef struct DU_WindowBootStrap{
    const char * name;
    DU_windowDevice *(*createDevice)(void);
};

DU_windowDevice * current_window_backend;

extern DU_WindowBootStrap x11WindowDevice;
extern DU_WindowBootStrap winWindowDevice;

DU_WindowBootStrap allWindowBootStraps[] = {
    x11WindowDevice,
    winWindowDevice
};

const unsigned int allWindowBootStrapsLength = 2; 

//function to initialize the backend and choose the correct device
int DU_Init(){
    for(unsigned int i = 0; i < allWindowBootStrapsLength; i++){
        DU_windowDevice * dev = allWindowBootStraps[i].createDevice();
        const char * devName = allWindowBootStraps[i].name;

        if(!dev || !dev->isAvailable(dev)){
            std::cout << devName << "is not an available backend on this system: skipping..." << std::endl;
            continue;
        }

        current_window_backend = dev;
        std::cout << devName << "successfully connected as a backend!" << std::endl;
        
        return 1;
    }

    std::cout << "No backends available!" << std::endl;
    return 0;
}


#endif