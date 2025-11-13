/**
 * @file window_core.h
 * @brief Declares basic code to interact with Desk Up
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
#include "desk_up_window_device.h"
#include "desk_up_window_bootstrap.h"

/**
 * @var std::string DESKUPDIR
 * \anchor DESKUPDIR_anchor
 * @brief The path to the DeskUp saved workspace.
 * 
 * @details This global variable gets assigned when calling DU_Init(), so using it without initializing DeskUp will cause undefined behaviour.
 * In the process, it calls the device's getDeskUpPath() function,
 * which is resolved to the specific backend implementation (e.g. WIN_getDeskUpPath on Windows).
 * 
 * @see DU_Init()
 * @see DeskUpWindowDevice::getDeskUpPath
 * @version 0.1.0
 * @date 2025 
 */
extern std::string DESKUPDIR;

/**
 * @var std::unique_ptr<DeskUpWindowDevice> current_window_backend
 * \anchor current_window_backend_anchor
 * @brief A unique pointer to the selected backend device for DeskUp.
 * 
 * @details This global pointer gets assigned when calling DU_Init(), so using it without initializing DeskUp will cause undefined behaviour.
 * It provides access to backend-specific functions such as window enumeration, size, and position.
 * 
 * @see DU_Init()
 * @see DeskUpWindowBootStrap
 * @version 0.1.0
 * @date 2025
 */
extern std::unique_ptr<DeskUpWindowDevice> current_window_backend;

/**
 * @brief Initializes the DeskUp backend system.
 * 
 * @details This function must be called once before using any DeskUp backend feature.
 * It iterates through the available backends (currently only the Windows backend) and:
 *  - Calls the backend bootstrap function `isAvailable()` to check if it can be used.
 *  - If available, calls `createDevice()` to create and configure the backend device.
 *  - Calls `getDeskUpPath()` through the device to determine the workspace base directory.
 *
 * Once initialization completes successfully:
 *  - The global variable \ref DESKUPDIR_anchor contains the DeskUp workspace path.
 *  - The global pointer \ref current_window_backend_anchor references the active backend device.
 *
 * @note The function currently supports only the Windows backend, which internally maps to:
 *  - @ref WIN_isAvailable()
 *  - @ref WIN_CreateDevice()
 *  - @ref WIN_getDeskUpPath()
 *
 * @return 1 if initialization succeeds, 0 otherwise.
 *
 * @see DeskUpWindowDevice
 * @see DeskUpWindowBootStrap
 * @see WIN_isAvailable()
 * @see WIN_CreateDevice()
 * @see WIN_getDeskUpPath()
 * @version 0.1.1
 * @date 2025
 */
int DU_Init();

#endif
