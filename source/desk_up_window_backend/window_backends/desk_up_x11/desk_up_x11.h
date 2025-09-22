#ifndef DESKUPX11_H
#define DESKUPX11_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
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

DeskUpWindowBootStrap x11WindowDevice = {
    "x11",
    X11_createDevice
};


DeskUpWindowDevice * X11_createDevice(void);

int X11_errorHandlerNonFatal(Display * display, XErrorEvent * event);

unsigned int X11_getWindowHeight(DeskUpWindowDevice * _this);
unsigned int X11_getWindowWidth(DeskUpWindowDevice * _this);
unsigned int X11_getWindowXPos(DeskUpWindowDevice * _this);
unsigned int X11_getWindowYPos(DeskUpWindowDevice * _this);

std::vector<windowDesc> X11_getAllWindows(DeskUpWindowDevice * _this);


#endif