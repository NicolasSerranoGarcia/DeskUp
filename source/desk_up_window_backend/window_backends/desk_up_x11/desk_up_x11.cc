#include "desk_up_x11.h"

bool X11_isAvailable(DU_WindowDevice * device){
    //X11 returns nullptr when it cannot stablish a connection with the user display
    Display * display = ((windowData *) device->internalData)->display;

    return display != nullptr;
}

DU_WindowDevice * X11_CreateDevice(void){
    //set all the functions of a DU_windowDevice variable to the functions of x11. Also set internalData to 

    DU_WindowDevice * device = nullptr;

    device->getWindowHeight = X11_getWindowHeight;
    device->isAvailable = X11_isAvailable;

    windowData * data;
    data->display = XOpenDisplay(NULL);

    device->internalData = data;
    
    return device;
}

unsigned int X11_getWindowHeight(DU_WindowDevice * _this){

    windowData * data = (windowData *) _this->internalData;

    if(data->display == nullptr || !data->w){
        throw std::invalid_argument("Invalid arguments!");
    }

    XWindowAttributes * returnAttributes;

    //loads returnAttributes with height
    if(!XGetWindowAttributes(data->display, data->w, returnAttributes)){
        throw std::runtime_error("X11 could not return the window height!");
    }

    const unsigned int height = returnAttributes->height;

    return height;
}

