#ifndef DESKUPWIN_H
#define DESKUPWIN_H

#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <stdlib.h>

#include "window_global.h"
#include "windowDesc.h"

// here we define API calls, like WIN_getWindowHeight()
//we also declare a struct data that carries specific information that the backend needs

typedef struct windowData{
    HWND hwnd;
};

DU_WindowBootStrap winWindowDevice = {
    "win",
    WIN_CreateDevice
};


DU_windowDevice * WIN_CreateDevice(void);
bool WIN_isAvailable(DU_windowDevice *);

unsigned int WIN_getWindowHeight(DU_windowDevice * _this);
unsigned int WIN_getWindowWidth(DU_windowDevice * _this);
unsigned int WIN_getWindowXPos(DU_windowDevice * _this);
unsigned int WIN_getWindowYPos(DU_windowDevice * _this);

char * WIN_GetPathFromWindow(DU_windowDevice * _this);

std::vector<windowDesc> WIN_getAllWindows(DU_windowDevice * _this);

#endif