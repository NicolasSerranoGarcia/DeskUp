#ifndef DESKUPWIN_H
#define DESKUPWIN_H

#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <stdlib.h>

#include "window_desc.h"
#include "desk_up_window_bootstrap.h"
#include "desk_up_window_device.h"

/**
 * This struct is an opaque type used to pass platform-specific info from a window to the backend calls. 
 * A void ptr is included in each device, and whenever 
 */
struct windowData;

DeskUpWindowDevice * WIN_CreateDevice(HWND deskUpHWND);

extern DeskUpWindowBootStrap winWindowDevice;

std::string WIN_getDeskUpPath();
unsigned int WIN_getWindowHeight(DeskUpWindowDevice * _this);
unsigned int WIN_getWindowWidth(DeskUpWindowDevice * _this);
unsigned int WIN_getWindowXPos(DeskUpWindowDevice * _this);
unsigned int WIN_getWindowYPos(DeskUpWindowDevice * _this);

std::string WIN_GetPathFromWindow(DeskUpWindowDevice * _this);

std::vector<windowDesc> WIN_getAllWindows(DeskUpWindowDevice * _this);



#endif