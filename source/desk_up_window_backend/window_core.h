/**
 * @file desk_up_window_bootstrap.h
 * @brief A struct wrapper used in the DeskUp backend
 *
 * This file is part of DeskUp
 *
 * @author
 *   Nicolas Serrano Garcia <serranogarcianicolas@gmail.com>
 * @date
 *   2025
 * @copyright
 *   Copyright (C) 2025 Nicolas Serrano Garcia
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef WINDOWGLOBAL_H
#define WINDOWGLOBAL_H

#include <iostream>
#include <vector>
#include "window_desc.h"
#include "desk_up_win.h"
#include "desk_up_window_device.h"
#include "desk_up_window_bootstrap.h"

/**
 * @brief The path to the desk Up saved workspace
 * 
 * @details This global variable gets assigned when calling DU_Init(), so using it without initializing DeskUp will cause undefined behaviour.
 * In the process, it calls the device getDeskUpPath() call,
 * which gets resolved to the specific backend function (like WIN_getDeskUpPath)
 * 
 * @see DU_Init()
 * @see DeskUpWindowDevice::getDeskUpPath
 * @version 0.1.0
 * @date 2025 
 */
extern std::string DESKUPDIR;

/**
 * @brief A unique pointer pointing to the select backend device for the program. It must be visible to the client code in order to call 
 * the backend functions.
 * 
 * @details This global pointer gets assigned when calling DU_Init(), so using it without initializing DeskUp will cause undefined behaviour.
 * 
 * @see DU_Init()
 * @see DeskUpWindowBootStrap
 * @version 0.1.0
 * @date 2025
 */
extern std::unique_ptr<DeskUpWindowDevice> current_window_backend;

/**
 * @brief A function to initialize DeskUp. It must be called every time you want to use DeskUp's backend.
 * 
 * Calls:
 * 
 * @details . To access backend functions, 
 * see current_window_backend. This function also sets DESKUPDIR to the correct path. 
 * 
 * @see DeskUpWindowDevice
 * @see DeskUpWindowBootStrap
 * @version 0.1.0
 * @date 2025
 */
int DU_Init();

#endif