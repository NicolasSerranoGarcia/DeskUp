#ifndef WINDOWGLOBAL_H
#define WINDOWGLOBAL_H

#include <iostream>
#include <vector>
#include "window_desc.h"
#include "desk_up_win.h"
#include "desk_up_window_device.h"
#include "desk_up_window_bootstrap.h"

extern std::string DESKUPDIR;

extern std::unique_ptr<DeskUpWindowDevice> current_window_backend;

//function to initialize the backend and choose the correct device. Previously used x11 and Windows, but now only connects windows
int DU_Init();

#endif