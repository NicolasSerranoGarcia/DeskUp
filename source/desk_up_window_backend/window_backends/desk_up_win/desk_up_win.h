/**
 * @file desk_up_win.h
 * @brief Bootstrap and functions for the Windows window backend (DeskUp)
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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
 * @brief Windows backend bootstrap descriptor.
 *
 * @details This global is used by \c DU_Init to determine whether Windows
 *          is available on the device and to create the corresponding
 *          \c DeskUpWindowDevice. After creation, all calls should go
 *          through the device instance (e.g., via \c current_window_device
 *          in \c window_core.h or equivalent).
 *
 * @see DeskUpWindowBootStrap
 * @see DeskUpWindowDevice
 * @version 0.1.0
 * @date 2025
 */
extern DeskUpWindowBootStrap winWindowDevice;

/**
 * @brief Returns whether the Windows backend is available.
 * @return \c true when compiled/executed on Windows, \c false otherwise.
 * @version 0.1.0
 * @date 2025
 */
bool WIN_isAvailable();

/**
 * @brief Creates a Windows \c DeskUpWindowDevice.
 *
 * @details Wires the device function pointers to the Windows backend
 *          implementations and allocates the internal data required.
 *
 * @return An initialized \c DeskUpWindowDevice.
 * @version 0.1.0
 * @date 2025
 */
DeskUpWindowDevice WIN_CreateDevice();

/**
 * @brief Returns the base DeskUp working path on the system.
 *
 * @details Points to the top-level directory where user workspaces are stored.
 *          The directory is ensured to exist (created if necessary).
 *
 * @return \c std::string with the absolute path to the DeskUp top-level folder.
 * @throws std::runtime_error If directory creation fails (message: "WIN_getDeskUpPath: <cause>").
 * @version 0.1.0
 * @date 2025
 */
std::string WIN_getDeskUpPath();

/**
 * @brief Gets the X position (top-left corner) of the active window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the X coordinate of the window.
 * @throws std::invalid_argument If the internal \c HWND is null/invalid
 *         (message: "WIN_getWindowXPos: invalid HWND").
 * @throws std::runtime_error If a Windows API call fails
 *         (message e.g., "WIN_getWindowXPos: GetWindowInfo: <cause>").
 * @version 0.1.0
 * @date 2025
 */
int WIN_getWindowXPos(DeskUpWindowDevice * _this);

/**
 * @brief Gets the Y position (top-left corner) of the active window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the Y coordinate of the window.
 * @throws std::invalid_argument If the internal \c HWND is null/invalid
 *         (message: "WIN_getWindowYPos: invalid HWND").
 * @throws std::runtime_error If a Windows API call fails
 *         (message e.g., "WIN_getWindowYPos: GetWindowInfo: <cause>").
 * @version 0.1.0
 * @date 2025
 */
int WIN_getWindowYPos(DeskUpWindowDevice * _this);

/**
 * @brief Gets the width of the active window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window width.
 * @throws std::invalid_argument If the internal \c HWND is null/invalid
 *         (message: "WIN_getWindowWidth: invalid HWND").
 * @throws std::runtime_error If a Windows API call fails
 *         (message e.g., "WIN_getWindowWidth: GetWindowInfo: <cause>").
 * @version 0.1.0
 * @date 2025
 */
unsigned int WIN_getWindowWidth(DeskUpWindowDevice * _this);

/**
 * @brief Gets the height of the active window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window height.
 * @throws std::invalid_argument If the internal \c HWND is null/invalid
 *         (message: "WIN_getWindowHeight: invalid HWND").
 * @throws std::runtime_error If a Windows API call fails
 *         (message e.g., "WIN_getWindowHeight: GetWindowInfo: <cause>").
 * @version 0.1.0
 * @date 2025
 */
unsigned int WIN_getWindowHeight(DeskUpWindowDevice * _this);

/**
 * @brief Gets the absolute path of the executable that owns the active window.
 *
 * @param _this The same device instance.
 * @return \c std::string with the process image path. If access is denied
 *         (e.g., \c ERROR_ACCESS_DENIED), an empty string is returned.
 * @throws std::invalid_argument If the internal \c HWND is null/invalid
 *         (message: "WIN_getPathFromWindow: invalid HWND").
 * @throws std::runtime_error If a system call fails
 *         (messages like "WIN_getPathFromWindow: <ApiName>: <cause>").
 * @version 0.1.0
 * @date 2025
 */
std::string WIN_getPathFromWindow(DeskUpWindowDevice * _this);

/**
 * @brief Enumerates all visible/non-minimized windows on the desktop.
 *
 * @param _this The same device instance.
 * @return \c std::vector<windowDesc> with the abstract description of each window.
 * @throws std::runtime_error If \c EnumDesktopWindows fails
 *         (message: "WIN_getAllWindows: <cause>").
 * @version 0.1.0
 * @date 2025
 */
std::vector<windowDesc> WIN_getAllWindows(DeskUpWindowDevice * _this);

/**
 * @brief Creates a process from the specified path. 
 *
 * @param _this The same device instance.
 * @param path a literal representing the path to the executable linked with the program
 * @return \c void
 * @throws
 * @version 0.1.0
 * @date 2025
 */
void WIN_loadProcessFromPath(DeskUpWindowDevice * _this, const char * path);

#endif
