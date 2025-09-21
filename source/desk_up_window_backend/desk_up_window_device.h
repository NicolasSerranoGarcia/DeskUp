#ifndef DESKUPWINDOWDEVICE_H
#define DESKUPWINDOWDEVICE_H

#include <vector>
#include <string>
#include "windowDesc.h"

struct DU_windowDevice{
    //here goes all the generic functions that all the backends have
    unsigned int (*getWindowHeight)(DU_windowDevice * _this);
    unsigned int (*getWindowWidth)(DU_windowDevice * _this);
    unsigned int (*getWindowXPos)(DU_windowDevice * _this);
    unsigned int (*getWindowYPos)(DU_windowDevice * _this);
    std::string (*getDeskUpPath)(void);
    
    std::vector<windowDesc> (*getAllWindows)(DU_windowDevice * _this);

    void * internalData;
};

#endif