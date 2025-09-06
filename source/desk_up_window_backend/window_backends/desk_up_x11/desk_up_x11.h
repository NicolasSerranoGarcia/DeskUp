#ifndef DESKUPX11_H
#define DESKUPX11_H

#include <X11/Xlib.h>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "window_global.h"
#include "windowDesc.h"

// here we define API calls, like x11_getWindowHeight()
//we also declare a struct data that carries specific information that the backend needs

typedef struct windowData{
    Display * display;
    Window w;
    Screen * screen;
};

DU_WindowBootStrap x11WindowDevice = {
    "x11",
    X11_createDevice
};


DU_windowDevice * X11_createDevice(void);
bool X11_isAvailable(DU_windowDevice * device);

int X11_errorHandlerNonFatal(Display * display, XErrorEvent * event);

unsigned int X11_getWindowHeight(DU_windowDevice * _this);
unsigned int X11_getWindowWidth(DU_windowDevice * _this);
unsigned int X11_getWindowXPos(DU_windowDevice * _this);
unsigned int X11_getWindowYPos(DU_windowDevice * _this);

std::vector<windowDesc> X11_getAllWindows(DU_windowDevice * _this);


#endif