/**
 * @file desk_up_backend_interface.h
 * @brief High-level window operations for DeskUp (frontend utilities over the backend device).
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

#ifndef DESKUPBACKENDINTERFACE_H
#define DESKUPBACKENDINTERFACE_H

#include <string>
#include <expected>
#include <filesystem>

#include "desk_up_error.h"

namespace fs = std::filesystem;

/**
 * @struct DeskUpWindow
 * @brief Convenience façade for workspace-level window operations.
 *
 * @details
 * This lightweight façade exposes high-level operations that orchestrate the
 * active backend device (see @ref current_window_backend) to work with
 * window snapshots and workspaces.
 *
 * Error handling:
 * - All functions now return `DeskUp::Status` or `DeskUp::Result<T>` values
 *   instead of throwing exceptions.
 * - Each function propagates `DeskUp::Error` instances describing
 *   severity (`Level`) and category (`ErrType`).
 * - Fatal errors stop the current operation immediately.
 * - Retry and Warning levels are non-fatal and allow continuation.
 *
 * @see DU_Init
 * @see DESKUPDIR
 * @see current_window_backend
 * @see DeskUpWindowDevice::getAllOpenWindows
 * @see windowDesc
 * @see windowDesc::saveTo
 * @version 0.1.1
 * @date 2025
 */
struct DeskUpWindow{

    /**
     * @brief Saves all currently enumerated windows to a local workspace folder.
     *
     * @details
     * Builds `<DESKUPDIR>/<workspaceName>` and ensures the directory exists.
     * Then asks the active backend device to enumerate all open windows and writes
     * each window's description to an individual file using `windowDesc::saveTo()`.
     * Non-fatal save errors are skipped; fatal ones abort the operation.
     *
     * **Calls (indirectly through the backend):**
     * - `DeskUpWindowDevice::getAllOpenWindows(DeskUpWindowDevice*)`
     * - `windowDesc::saveTo(const std::string&)`
     *
     * **Reads:**
     * - @ref DESKUPDIR (must have been set by a prior @ref DU_Init call).
     *
     * @param workspaceName Name of the workspace folder to create/use under @ref DESKUPDIR.
     * @return `DeskUp::Status` — empty on success, or `std::unexpected(DeskUp::Error)` on failure.
     *
     * @errors
     * - Level::Fatal, ErrType::Os or ErrType::InvalidInput → Enumeration or write failure.
     * - Level::Warning → One or more windows skipped (non-fatal save errors).
     * - Level::Retry → Temporary filesystem access problems.
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     * @version 0.1.1
     * @date 2025
     */
    static DeskUp::Status saveAllWindowsLocal(std::string workspaceName);

    /**
     * @brief Restores all tabs saved previously in the workspace name specified by the parameter.
     *
     * @details
     * Iterates over all files in `<DESKUPDIR>/<workspaceName>` and, for each saved window:
     * 1. Loads the window description from file (`recoverSavedWindow`).
     * 2. Closes existing process instances of that executable (`closeProcessFromPath`).
     * 3. Launches a new process (`loadWindowFromPath`).
     * 4. Resizes the new window to the stored geometry (`resizeWindow`).
     *
     * Non-fatal backend errors (Retry or Warning) are logged to console but do not abort
     * the overall restore cycle. Fatal errors propagate as a failed `DeskUp::Status`.
     *
     * **Calls (indirectly through the backend):**
     * - `DeskUpWindowDevice::recoverSavedWindow`
     * - `DeskUpWindowDevice::closeProcessFromPath`
     * - `DeskUpWindowDevice::loadWindowFromPath`
     * - `DeskUpWindowDevice::resizeWindow`
     *
     * **Reads:**
     * - @ref DESKUPDIR (workspace base directory).
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return `DeskUp::Status` — empty on success, or `std::unexpected(DeskUp::Error)` on failure.
     *
     * @errors
     * - Level::Fatal, ErrType::NotFound → Workspace directory missing.
     * - Level::Fatal, ErrType::InvalidInput → Corrupted or incomplete window file.
     * - Level::Retry, ErrType::NotFound → Process launched but no main HWND found.
     * - Level::Warning → Individual window restore failed but continued.
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     * @version 0.1.1
     * @date 2025
     */
    static DeskUp::Status restoreWindows(std::string workspaceName);

    /**
     * @brief This function checks whether if a string is a valid name for a workspace folder.
     *
     * @details
     * A workspace name is invalid if it is empty or contains any of the following
     * forbidden characters: `\\ / : ? * " < > |`.
     *
     * **Reads:**
     * - Pure string validation, does not access filesystem.
     *
     * @param workspaceName Name of the workspace to check.
     * @return \c true if the workspace name is valid, \c false otherwise.
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     * @version 0.1.1
     * @date 2025
     */
    static bool isWorkspaceValid(const std::string& workspaceName);

    /**
     * @brief This function checks whether if a given workspace with the name \c workspaceName already exists.
     *
     * @details
     * Checks whether `<DESKUPDIR>/<workspaceName>` exists and is a directory.
     *
     * **Reads:**
     * - Filesystem state under @ref DESKUPDIR.
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return \c true if the workspace exists, \c false otherwise.
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     * @version 0.1.1
     * @date 2025
     */
    static bool existsWorkspace(const std::string& workspaceName);

    /**
     * @brief This function deletes a workspace.
     *
     * @details
     * Removes `<DESKUPDIR>/<workspaceName>` recursively from disk.
     * Returns `1` on success, `0` if the workspace does not exist or deletion failed.
     *
     * **Reads:**
     * - Filesystem state under @ref DESKUPDIR.
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return \c 1 if deleted, \c 0 otherwise.
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     * @version 0.1.1
     * @date 2025
     */
    static int removeWorkspace(const std::string& workspaceName);

    /**
     * @brief Checks whether a given file path exists on disk.
     *
     * @details
     * Uses the C++17 `<filesystem>` library to test whether the file or directory
     * specified by `filePath` exists in the current filesystem.
     * Returns `true` if it exists, `false` otherwise.
     *
     * **Reads:**
     * - Filesystem state at the given @p filePath.
     *
     * @param filePath The absolute or relative filesystem path to check.
     * @return \c true if the file or directory exists, \c false otherwise.
     *
     * @note This function is typically used when generating unique file names
     *       for saving window descriptors (to avoid overwriting files when multiple
     *       windows share the same name).
     *
     * @version 0.1.1
     * @date 2025
     */
    static bool existsFile(const fs::path& filePath);

};

#endif
