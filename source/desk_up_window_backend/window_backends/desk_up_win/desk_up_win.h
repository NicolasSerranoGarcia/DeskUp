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

#include <stdexcept>
#include <vector>
#include <filesystem>

#include <stdlib.h>
#include <Windows.h>

#include "desk_up_window_bootstrap.h"
#include "desk_up_window_device.h"
#include "window_desc.h"
#include "desk_up_error.h"

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
bool WIN_isAvailable() noexcept;

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
 * @errors
 * - Level::Retry, ErrType::Io → Filesystem creation failed (non-fatal)
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<std::string> WIN_getDeskUpPath();

/**
 * @brief Gets the X position (top-left corner) of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the X coordinate of the window.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid HWND.
 * - Level::Retry, ErrType::Os → GetWindowInfo failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<int> WIN_getWindowXPos(DeskUpWindowDevice * _this);

/**
 * @brief Gets the Y position (top-left corner) of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the Y coordinate of the window.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid HWND.
 * - Level::Retry, ErrType::Os → GetWindowInfo failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<int> WIN_getWindowYPos(DeskUpWindowDevice * _this);

/**
 * @brief Gets the width of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window width.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid HWND.
 * - Level::Retry, ErrType::Os → GetWindowInfo failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<unsigned int> WIN_getWindowWidth(DeskUpWindowDevice * _this);

/**
 * @brief Gets the height of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window height.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid HWND.
 * - Level::Retry, ErrType::Os → GetWindowInfo failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<unsigned int> WIN_getWindowHeight(DeskUpWindowDevice * _this);

/**
 * @brief Gets the absolute path of the executable that owns the active window.
 *
 * @param _this The same device instance.
 * @return \c std::string with the process image path. If access is denied
 *         (e.g., \c ERROR_ACCESS_DENIED), an empty string is returned.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid HWND.
 * - Level::Retry, ErrType::Os → OpenProcess or QueryFullProcessImageName failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<std::string> WIN_getPathFromWindow(DeskUpWindowDevice * _this);

/**
 * @brief Enumerates all visible/non-minimized windows on the desktop.
 *
 * @param _this The same device instance.
 * @return \c std::vector<windowDesc> with the abstract description of each window.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Device invalid.
 * - Level::Retry, ErrType::Os → EnumDesktopWindows failed.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<std::vector<windowDesc>> WIN_getAllOpenWindows(DeskUpWindowDevice * _this);

/**
 * @brief Loads a window description from a saved workspace file.
 *
 * @param _this The same device instance.
 * @param path Path to the saved window description file.
 * @return \c windowDesc with geometry and executable path.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → File missing or incomplete.
 * - Level::Retry, ErrType::Io → Filesystem or parse failure.
 * @version 0.2.0
 * @date 2025
 */
DeskUp::Result<windowDesc> WIN_recoverSavedWindow(DeskUpWindowDevice * _this, std::filesystem::path path) noexcept;

/**
 * @brief Creates a process from the specified path.
 *
 * @param _this The same device instance.
 * @param path a literal representing the path to the executable linked with the program.
 * @return \c DeskUp::Status indicating success or failure.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Empty or invalid path/device.
 * - Level::Retry, ErrType::NotFound → Process started but main HWND not found.
 * - Level::Retry, ErrType::Os → ShellExecuteEx failed.
 * @version 0.2.0
 * @date 2025
 */
DeskUp::Status WIN_loadProcessFromPath(DeskUpWindowDevice * _this, std::string path) noexcept;

/**
 * @brief Resizes a window according to the windowDesc parameter geometry.
 *
 * @details Information about the window whose geometry is intended to modify must be specified inside the \c _this->internalData parameter.
 *
 * @param _this The same device instance.
 * @param window a windowDesc instance whose geometry will be applied to resize the window.
 * @return \c DeskUp::Status indicating success or failure.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Invalid window device.
 * - Level::Retry, ErrType::NotFound → HWND not found.
 * - Level::Warning, ErrType::InvalidInput → Zero or negative width/height.
 * - Level::Retry, ErrType::Os → SetWindowPos failed.
 * @version 0.2.0
 * @date 2025
 */
DeskUp::Status WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window);

/**
 * @brief This function closes all the instances associated with an executable, specified by the \c path parameter.
 *
 * @param _this The same device instance.
 * @param path A \c std::string instance representing the path to check.
 * @param allowForce Whether if the call should force the windows it finds to close.
 * @return \c The number of associated windows closed in the process.
 * @errors
 * - Level::Fatal, ErrType::InvalidInput → Empty path.
 * - Level::Retry, ErrType::Os → Process enumeration or termination failure.
 * @version 0.2.0
 * @date 2025
 */
DeskUp::Result<unsigned int> WIN_closeProcessFromPath(DeskUpWindowDevice*, const std::string& path, bool allowForce);

#endif
