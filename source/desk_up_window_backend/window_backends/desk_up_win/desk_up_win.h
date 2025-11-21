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

namespace fs = std::filesystem;

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
DeskUpWindowDevice WIN_CreateDevice() noexcept;

/**
 * @brief Deletes a Windows \c DeskUpWindowDevice. Deletion is done as a wrapper function in window_core.cc,
 * done by DU_destroy via the pointer function
 *
 * @version 0.3.2
 * @date 2025
 */
void WIN_destroyDevice(DeskUpWindowDevice* _this) noexcept;

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
DeskUp::Result<std::string> WIN_getDeskUpPath() noexcept;

/**
 * @brief Gets the X position (top-left corner) of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the X coordinate of the window.
 * @errors
 * - Level::Error, ErrType::DeviceNotFound → Missing or invalid device/internal data (explicit check).
 * - Level::Skip, ErrType::InvalidInput → No valid HWND bound (explicit check).
 * @note Indirect errors: This function wraps `GetWindowInfo` with `retryOp`, which may propagate additional system-derived errors (fatal/skip/warning/retry) classified via `Error::fromLastWinError`. Those are not enumerated here because they are not produced directly by this function but rethrown from `retryOp`.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<int> WIN_getWindowXPos(DeskUpWindowDevice * _this) noexcept;

/**
 * @brief Gets the Y position (top-left corner) of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c int with the Y coordinate of the window.
 * @errors
 * - Level::Error, ErrType::DeviceNotFound → Missing or invalid device/internal data (explicit check).
 * - Level::Skip, ErrType::InvalidInput → No valid HWND bound (explicit check).
 * @note Indirect errors: Uses `retryOp` around `GetWindowInfo`; any Windows-origin errors are classified and rethrown by `retryOp` (see `Error::fromLastWinError`). They are not listed here.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<int> WIN_getWindowYPos(DeskUpWindowDevice * _this) noexcept;

/**
 * @brief Gets the width of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window width.
 * @errors
 * - Level::Error, ErrType::DeviceNotFound → Missing or invalid device/internal data (explicit check).
 * - Level::Skip, ErrType::InvalidInput → No valid HWND bound (explicit check).
 * @note Indirect errors: Additional system errors may be returned via `retryOp` wrapping `GetWindowInfo`; see its implementation and `Error::fromLastWinError` for classification.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<unsigned int> WIN_getWindowWidth(DeskUpWindowDevice * _this) noexcept;

/**
 * @brief Gets the height of the active (client) window in the device.
 *
 * @param _this The same device instance.
 * @return \c unsigned \c int with the window height.
 * @errors
 * - Level::Error, ErrType::DeviceNotFound → Missing or invalid device/internal data (explicit check).
 * - Level::Skip, ErrType::InvalidInput → No valid HWND bound (explicit check).
 * @note Indirect errors: Relies on `retryOp` for `GetWindowInfo`; propagated errors are produced by `retryOp` and not enumerated here.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<unsigned int> WIN_getWindowHeight(DeskUpWindowDevice * _this) noexcept;

/**
 * @brief Gets the absolute path of the executable that owns the active window.
 *
 * @param _this The same device instance.
 * @return \c fs::path with the process image path on success.
 * @errors
 * - Level::Error, ErrType::DeviceNotFound → Missing or invalid device/internal data (explicit check).
 * - Level::Skip, ErrType::InvalidInput → No valid HWND bound (explicit check).
 * @note Indirect errors (via `retryOp`): This function internally retries three Windows API sequences: `GetWindowThreadProcessId`, `OpenProcess` + `GetExitCodeProcess`, and path resolution (e.g. `QueryFullProcessImageName`). Failures inside those lambdas are converted by `Error::fromLastWinError`. The most relevant Windows codes and their mapped DeskUp classifications for this routine are:
 *   - `ERROR_INVALID_WINDOW_HANDLE` → Level::Skip, ErrType::ConnectionRefused (invalid/closed HWND when obtaining PID).
 *   - `ERROR_ACCESS_DENIED` (transformed to `ERROR_ACCESS_DISABLED_BY_POLICY` before returning) → Level::Skip, ErrType::Default (policy or permission restriction when opening the process).
 *   - `ERROR_NOT_ENOUGH_MEMORY` / `ERROR_OUTOFMEMORY` / `ERROR_TOO_MANY_OPEN_FILES` → Level::Fatal, ErrType::InsufficientMemory (system resource exhaustion during any queried step).
 *   - `ERROR_SHARING_VIOLATION`, `ERROR_INVALID_PARAMETER`, `ERROR_INVALID_NAME`, `ERROR_FILENAME_EXCED_RANGE`, `ERROR_BAD_FORMAT` → Level::Skip, ErrType::InvalidInput (unexpected invalid handle/arguments surfaced by underlying APIs).
 *   - `ERROR_FILE_NOT_FOUND`, `ERROR_PATH_NOT_FOUND` → Level::Skip, ErrType::InvalidInput (executable image could not be resolved).
 *   - `ERROR_DISK_FULL` → Level::Fatal, ErrType::Io (unlikely here, but propagated if encountered reading image name).
 *   - `ERROR_WRITE_FAULT` / `ERROR_READ_FAULT` / `ERROR_CRC` / `ERROR_IO_DEVICE` / `ERROR_FUNCTION_FAILED` → Level::Retry, ErrType::Io or ErrType::Unexpected (transient or unexpected low-level I/O issues; retries attempted before surfacing).
 *   - Other unmapped codes → Level::Default, ErrType::Default.
 * These are not emitted directly by `WIN_getPathFromWindow`; they are rethrown from `retryOp` after classification. Only the explicit pre-check errors are listed in the main @errors section above.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<fs::path> WIN_getPathFromWindow(DeskUpWindowDevice * _this) noexcept;

/**
 * @brief Enumerates all visible/non-minimized windows on the desktop.
 *
 * @param _this The same device instance.
 * @return \c std::vector<windowDesc> with the abstract description of each window.
 * @errors
 * - Level::Fatal, ErrType::Unexpected → Device or windowData became corrupt during enumeration (explicit check after EnumDesktopWindows fails and error.level() == Error).
 * - Level::Warning, ErrType::Unexpected → EnumDesktopWindows returned false for an unexpected reason not classified as fatal/error (catch-all for unanticipated callback failures).
 * @note Indirect errors (via `WIN_CreateAndSaveWindowProc` callback): During enumeration, each window is processed via a callback that invokes `WIN_getPathFromWindow`, `WIN_getWindowXPos`, `WIN_getWindowYPos`, `WIN_getWindowWidth`, and `WIN_getWindowHeight`. The callback may generate:
 *   - Level::Error, ErrType::InvalidInput → Callback parameters missing (improbable).
 *   - Level::Error, ErrType::DeviceNotFound → Missing device/internal data while processing a window.
 *   - Fatal errors from any geometry/path call → Bubble up immediately, aborting enumeration.
 *   - Skip errors (e.g., invalid HWND mid-enumeration) → Individual window skipped, enumeration continues.
 *   - Other Error-level issues → Tolerated once (static flag `levelErrorHappened`), fatal on second consecutive occurrence.
 * Since the geometry and path functions each call `retryOp`, refer to their individual documentation (`WIN_getWindowXPos`, `WIN_getWindowYPos`, `WIN_getWindowWidth`, `WIN_getWindowHeight`, `WIN_getPathFromWindow`) for complete Windows error code mappings that may propagate through the callback.
 * @version 0.1.0
 * @date 2025
 */
DeskUp::Result<std::vector<windowDesc>> WIN_getAllOpenWindows(DeskUpWindowDevice * _this) noexcept;

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
DeskUp::Result<windowDesc> WIN_recoverSavedWindow(DeskUpWindowDevice * _this, const fs::path& path) noexcept;

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
DeskUp::Status WIN_loadProcessFromPath(DeskUpWindowDevice * _this, const fs::path& path) noexcept;

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
DeskUp::Status WIN_resizeWindow(DeskUpWindowDevice * _this, const windowDesc window) noexcept;

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
DeskUp::Result<unsigned int> WIN_closeProcessFromPath(DeskUpWindowDevice*, const fs::path& path, bool allowForce) noexcept;

/**
 * @brief Test-only helper to set the internal HWND for the device.
 * @param _this The device instance.
 * @param hwnd The HWND to assign.
 * @note Only available when DESKUP_ENABLE_WIN32_TEST_HOOKS is defined.
 */
void WIN_TEST_setHWND(DeskUpWindowDevice* _this, HWND hwnd);

#endif
