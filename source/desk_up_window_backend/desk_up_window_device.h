#ifndef DESKUPWINDOWDEVICE_H
#define DESKUPWINDOWDEVICE_H

#include <vector>
#include <string>
#include "windowDesc.h"

struct DeskUpWindowDevice{
    //here goes all the generic functions that all the backends have
    unsigned int (*getWindowHeight)(DeskUpWindowDevice * _this);
    unsigned int (*getWindowWidth)(DeskUpWindowDevice * _this);
    unsigned int (*getWindowXPos)(DeskUpWindowDevice * _this);
    unsigned int (*getWindowYPos)(DeskUpWindowDevice * _this);
    std::string (*getDeskUpPath)(void);
    
    std::vector<windowDesc> (*getAllWindows)(DeskUpWindowDevice * _this);

    void * internalData;
};

#endif