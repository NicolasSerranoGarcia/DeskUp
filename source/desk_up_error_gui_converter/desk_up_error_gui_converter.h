/**
 * @file desk_up_error_gui_converter_qt.h
 * @brief Adapter to convert DeskUp internal errors into Qt GUI dialog messages.
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
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DESKUPERRORGUICONVERTER_H
#define DESKUPERRORGUICONVERTER_H

#include <QMessageBox>
#include <QString>
#include "desk_up_error.h"

namespace DeskUp::UI {

    /**
     * @class ErrorAdapter
     * @brief Utility class to bridge DeskUp's error system with the Qt graphical interface.
     *
     * @details
     * Converts `DeskUp::Error` instances into localized and user-friendly
     * messages displayed via `QMessageBox`.
     *
     * - Translates error **levels** into dialog icons and titles
     * - Translates error **types** into human-readable messages
     * - Displays the result via `QMessageBox`
     *
     * @see DeskUp::Error
     * @see Level
     * @see ErrType
     * @version 0.3.0
     * @date 2025
     */
    class ErrorAdapter {
    public:
        /**
         * @brief Displays a Qt dialog box for a given error.
         * @param err A @ref DeskUp::Error instance to display.
         * @return Integer result code from `QMessageBox::exec()`.
         */
        static int showError(const DeskUp::Error& err);

        static QString getUserMessage(const DeskUp::Error& err);
        static std::pair<QString, QMessageBox::Icon> mapLevel(Level lvl);

    };
}

#endif
