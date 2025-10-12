/**
 * @file desk_up_window.h
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

#ifndef DESKUPWINDOW_H
#define DESKUPWINDOW_H

#include <string>

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
 * - Backend enumeration failures are caught and result in a `0` return value
 *   (the exception is logged; no rethrow).
 * - Unexpected exceptions are also caught and mapped to `0`.
 * - Directory creation uses the standard library; failures may throw
 *   `std::filesystem::filesystem_error` if the platform reports an error and it
 *   is not caught in the implementation.
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
     * Then asks the active backend device to enumerate all windows and writes
     * each window's description to an individual file using
     * `windowDesc::saveTo()`. Backend errors are handled internally.
     *
     * **Calls (indirectly through the backend):**
     * - `DeskUpWindowDevice::getAllOpenWindows(DeskUpWindowDevice*)`
     * - `windowDesc::saveTo(const std::string&)`
     *
     * **Reads:**
     * - @ref DESKUPDIR (must have been set by a prior @ref DU_Init call).
     *
     * @param workspaceName Name of the workspace folder to create/use under @ref DESKUPDIR.
     * @return `1` on success (even if some files may be skipped due to per-window failures),
     *         `0` if a backend error or unexpected error occurs during enumeration.
     *
     * @throws std::filesystem::filesystem_error If the workspace directory cannot be created
     *         by the underlying filesystem operation (not caught inside this function).
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     */
    static int saveAllWindowsLocal(std::string workspaceName);

    /**
     * @brief restores all tabs saved previously in the workspace name specified by the parameter.
     *
     * @details
     *
     * **Calls (indirectly through the backend):**
     *
     * **Reads:**
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return `1` on success (even if some files may be skipped due to per-window failures),
     *         `0` if a backend error or unexpected error occurs during enumeration.
     *
     * @throws
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     */
    static int restoreWindows(std::string workspaceName);

    /**
     * @brief This function checks whether if a string is a valid name for a workspace folder.
     *
     * @details
     *
     * **Calls (indirectly through the backend):**
     *
     * **Reads:**
     *
     * @param workspaceName Name of the workspace to check
     * @return \c true if the workspace name is valid, \c false otherwise
     *
     * @throws
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     */
    static bool isWorkspaceValid(const std::string& workspaceName);

    /**
     * @brief This function checks whether if a given workspace with the name \c workspaceName already exists
     *
     * @details
     *
     * **Calls (indirectly through the backend):**
     *
     * **Reads:**
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return \c true if the workspace exists, \c false otherwise
     *
     * @throws
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     */
    static bool existsWorkspace(const std::string& workspaceName);

    /**
     * @brief This function deletes a workspace
     *
     * @details
     *
     * **Calls (indirectly through the backend):**
     *
     * **Reads:**
     *
     * @param workspaceName Name of the workspace folder to use under @ref DESKUPDIR.
     * @return \c true if the workspace was deleted, \c false otherwise
     *
     * @throws
     *
     * @note Ensure @ref DU_Init has been called successfully before invoking this method so that
     *       @ref DESKUPDIR and @ref current_window_backend are properly initialized.
     */
    static int deleteWorkspace(const std::string& workspaceName);
};

#endif