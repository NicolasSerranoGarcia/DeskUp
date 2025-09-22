#ifndef DESKUPWINDOWBOOSTRAP_H
#define DESKUPWINDOWBOOSTRAP_H

#include "desk_up_window_device.h"
#include <Windows.h>

struct DeskUpWindowBootStrap{
    const char * name;
    DeskUpWindowDevice * (*createDevice)(HWND);
};

#endif