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


DU_WindowDevice * WIN_CreateDevice(void);
bool WIN_isAvailable(DU_WindowDevice *);

unsigned int WIN_getWindowHeight(DU_WindowDevice * _this);
unsigned int WIN_getWindowWidth(DU_WindowDevice * _this);
unsigned int WIN_getWindowXPos(DU_WindowDevice * _this);
unsigned int WIN_getWindowYPos(DU_WindowDevice * _this);

char * WIN_GetPathFromWindow(DU_WindowDevice * _this);

std::vector<windowDesc> WIN_getAllWindows(DU_WindowDevice * _this);

#endif