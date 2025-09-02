#ifndef DESKUPX11_H
#define DESKUPX11_H

#include <X11/Xlib.h>
#include <stdexcept>

#include "window_global.h"

// here we define API calls, like x11_getWindowHeight()
//we also declare a struct data that carries specific information that the backend needs

typedef struct windowData{
    Display * display;
    Window w;
    Screen screen;
};

DU_WindowBootStrap x11WindowDevice = {
    "x11",
    X11_CreateDevice
};


DU_WindowDevice * X11_CreateDevice(void);
int X11_isAvailable(void);

unsigned int X11_getWindowHeight(DU_WindowDevice * _this);

#endif