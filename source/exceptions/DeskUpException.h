/**
 * @file window_core.h
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

#ifndef DESKUPEXCEPTION_H
#define DESKUPEXCEPTION_H

#include <string>
#include <stdexcept>

namespace DeskUpEx {

    class AccessDenied final : public std::runtime_error{
        public:
        private:
    };

    class WorkspaceNotExists final : public std::runtime_error{
        public:
        private:
    };

    class DeskUpFileNotExists final : public std::runtime_error{
        public:
        private:
    };

    class workspaceAlreadyExists final : public std::runtime_error{
        public:
        private:
    };

}

#endif