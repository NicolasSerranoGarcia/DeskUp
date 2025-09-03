#ifndef DESKUPWIN_H
#define DESKUPWIN_H

#include <stdexcept>
#include <Windows.h>

#include "window_global.h"

// here we define API calls, like WIN_getWindowHeight()
//we also declare a struct data that carries specific information that the backend needs

typedef struct windowData{
    HWND hwnd;
};

DU_WindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice
};


DU_WindowDevice * WIN_CreateDevice(void);
bool WIN_isAvailable(DU_WindowDevice *);

unsigned int WIN_getWindowHeight(DU_WindowDevice * _this);

#endif