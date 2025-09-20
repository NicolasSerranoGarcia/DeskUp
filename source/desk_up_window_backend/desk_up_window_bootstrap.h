#ifndef DESKUPWINDOWBOOSTRAP_H
#define DESKUPWINDOWBOOSTRAP_H

#include "desk_up_window_device.h"
#include <Windows.h>

struct DU_WindowBootStrap{
    const char * name;
    DU_windowDevice * (*createDevice)(HWND);
};

#endif