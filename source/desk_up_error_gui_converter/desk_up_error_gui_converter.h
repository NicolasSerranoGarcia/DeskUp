/**
 * @file desk_up_error_gui_converter.h
 * @brief A helpful tool to convert from Desk Up errors to GUI errors
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

#ifndef DESKUPERRORGUICONVERTER_H
#define DESKUPERRORGUICONVERTER_H

#include <wx/msgdlg.h>
#include "desk_up_error.h"

namespace DeskUp::UI {

    class ErrorAdapter {
    public:
        static int showError(const DeskUp::Error& err);
        static wxString getUserMessage(const DeskUp::Error& err);

    private:
        static std::pair<wxString, long> mapLevel(Level lvl);
    };

} 

#endif