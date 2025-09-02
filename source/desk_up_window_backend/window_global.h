#ifndef WINDOWGLOBAL_H
#define WINDOWGLOBAL_H

#include <iostream>

typedef struct DU_WindowDevice{
    //here goes all the generic functions that all the backends have
    unsigned int (*getWindowHeight)(DU_WindowDevice * _this);
    int (*isAvailable)(void);


    void * internalData;
};

typedef struct DU_WindowBootStrap{
    const char * name;
    DU_WindowDevice *(*createDevice)(void);
};

DU_WindowDevice * current_window_backend;

extern DU_WindowBootStrap x11WindowDevice;

DU_WindowBootStrap allWindowBootStraps[] = {
    x11WindowDevice
};

const unsigned int allWindowBootStrapsLength = 1; 

//function to initialize the backend and choose the correct device
int DU_Init(){
    for(unsigned int i = 0; i < allWindowBootStrapsLength; i++){
        DU_WindowDevice * dev = allWindowBootStraps[i].createDevice();
        const char * devName = allWindowBootStraps[i].name;

        if(!dev || !dev->isAvailable()){
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