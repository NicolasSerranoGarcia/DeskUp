#ifndef DESKUPWIN_H
#define DESKUPWIN_H

#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <stdlib.h>

#include "windowDesc.h"
#include "desk_up_window_bootstrap.h"
#include "desk_up_window_device.h"

// here we define API calls, like WIN_getWindowHeight()
//we also declare a struct data that carries specific information that the backend needs

struct DU_WindowBootStrap;
struct DU_windowDevice;

struct windowData;

DU_windowDevice * WIN_CreateDevice(HWND deskUpHWND);

extern DU_WindowBootStrap winWindowDevice;

std::string WIN_getDeskUpPath();
unsigned int WIN_getWindowHeight(DU_windowDevice * _this);
unsigned int WIN_getWindowWidth(DU_windowDevice * _this);
unsigned int WIN_getWindowXPos(DU_windowDevice * _this);
unsigned int WIN_getWindowYPos(DU_windowDevice * _this);

std::string WIN_GetPathFromWindow(DU_windowDevice * _this);

std::vector<windowDesc> WIN_getAllWindows(DU_windowDevice * _this);



#endif