/**
 * @file desk_up_error_gui_converter.h
 * @brief Adapter to convert DeskUp internal errors into GUI dialog messages.
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

#ifndef DESKUPERRORGUICONVERTER_H
#define DESKUPERRORGUICONVERTER_H

#include <wx/msgdlg.h>
#include "desk_up_error.h"

namespace DeskUp::UI {

    /**
     * @class ErrorAdapter
     * @brief Utility class to bridge DeskUp's error system with the graphical user interface.
     *
     * @details
     * Converts `DeskUp::Error` instances into localized and user-friendly
     * messages displayed via `wxMessageBox`.
     *
     * This adapter serves as a minimal GUI front-end layer that:
     * - Translates error **levels** into dialog styles/icons (see @ref mapLevel)
     * - Translates error **types** into human-readable text (see @ref getUserMessage)
     * - Invokes `wxMessageBox()` for display (see @ref showError)
     *
     * @see DeskUp::Error
     * @see Level
     * @see ErrType
     * @version 0.2.1
     * @date 2025
     */
    class ErrorAdapter {
    public:
        /**
         * @brief Displays a GUI message box for a given error.
         *
         * @details
         * Determines the appropriate dialog title and icon style based on the
         * error level, then resolves a user-friendly message string using
         * @ref getUserMessage.  
         * Finally, invokes `wxMessageBox()` and returns its result code.
         *
         * @param err A @ref DeskUp::Error instance to display.
         * @return Integer result code returned by `wxMessageBox()`.
         *
         * @see mapLevel
         * @see getUserMessage
         * @see wxMessageBox
         */
        static int showError(const DeskUp::Error& err);

        /**
         * @brief Converts a @ref DeskUp::Error into a readable string for the user.
         *
         * @details
         * Provides contextual and human-readable explanations for common
         * DeskUp error types (e.g., access denied, file not found, etc.).
         * Falls back to the raw error message from `err.what()` for
         * unrecognized types.
         *
         * @param err A @ref DeskUp::Error whose description should be translated.
         * @return A localized `wxString` suitable for GUI display.
         */
        static wxString getUserMessage(const DeskUp::Error& err);

    private:
        /**
         * @brief Maps an error level to an appropriate dialog title and icon style.
         *
         * @details
         * Converts a `DeskUp::Level` into:
         * - A dialog title (`wxString`)
         * - A style flag (`long`) combining `wxICON_*` and `wxOK`.
         *
         * For example:
         * - `Level::Fatal` → "Critical Error" + `wxICON_ERROR | wxOK`
         * - `Level::Warning` → "Warning" + `wxICON_WARNING | wxOK`
         * - `Level::Info` → "Information" + `wxICON_INFORMATION | wxOK`
         *
         * @param lvl The @ref Level of the error.
         * @return A `std::pair<wxString, long>` containing the title and style.
         *
         * @see Level
         */
        static std::pair<wxString, long> mapLevel(Level lvl);
    };

}

#endif
