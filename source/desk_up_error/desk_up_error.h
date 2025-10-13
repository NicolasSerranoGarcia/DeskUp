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
#include <Windows.h>
#include <stdexcept>
#include <optional>
#include "backend_utils.h"

namespace DeskUp {

    enum class Level{
        Fatal,
        Retry,
        Default,
        None
    };

    enum class ErrType{
        InsufficientMemory, 
        AccessDenied,
        SharingViolation, 
        Io, 
        InvalidInput, 
        Unexpected,
        Default,
        None
    };

    class Error final : public std::runtime_error{
        public:
            Error() : std::runtime_error(""), lvl(Level::None), errType(ErrType::None), retries(0){};
            Error(Level l, ErrType err, unsigned int t, std::string msg) : std::runtime_error(std::move(msg)), 
                                                                                            lvl(l), errType(err), retries(t){};
            
            ErrType type() const noexcept { return errType; }
            Level level() const noexcept { return lvl; }
            int attempts() const noexcept { return retries; }
            windowDesc whichWindow() const noexcept { return affectedWindow; };

            bool isFatal() const noexcept { return lvl == Level::Fatal; }
            bool isRetriable() const noexcept { return lvl == Level::Retry; }
            explicit operator bool() const noexcept{ return lvl != Level::None; };

            //Only for windows
            //This function should be automated to convert the windows error to the specific Level, ErrType. 
            //For this, it shouldn't receive anything. If the user wants to create a specific combination of error, then it should
            //do so from the constructor. Here, for example, the AccessDenied error always associated with the Fatal Level. 
            //If a different association wanted to be made, then the constructor should be used.
            //optionally, if the error needs to carry information about how many times it has tried to execute a function, it will be passed
            static Error fromLastWinError(std::string_view context = "", std::optional<unsigned int> tries = std::nullopt){
                Level   lvl = Level::Default;
                ErrType typ = ErrType::Unexpected;

                DWORD code = GetLastError();

                switch (code) {
                    case ERROR_ACCESS_DENIED:
                    case ERROR_PRIVILEGE_NOT_HELD:
                        lvl = Level::Fatal; typ = ErrType::AccessDenied; break;

                    case ERROR_NOT_ENOUGH_MEMORY:
                    case ERROR_OUTOFMEMORY:
                        lvl = Level::Fatal; typ = ErrType::InsufficientMemory; break;

                    case ERROR_SHARING_VIOLATION:
                    case ERROR_LOCK_VIOLATION:
                        lvl = Level::Retry; typ = ErrType::SharingViolation; break;

                    case ERROR_INVALID_PARAMETER:
                    case ERROR_INVALID_NAME:
                    case ERROR_FILENAME_EXCED_RANGE:
                        lvl = Level::Fatal; typ = ErrType::InvalidInput; break;

                    case ERROR_FILE_NOT_FOUND:
                    case ERROR_PATH_NOT_FOUND:
                        lvl = Level::Default; typ = ErrType::InvalidInput; break;

                    case ERROR_DISK_FULL:
                        lvl = Level::Fatal; typ = ErrType::Io; break;
                    case ERROR_WRITE_PROTECT:
                    case ERROR_WRITE_FAULT:
                    case ERROR_READ_FAULT:
                    case ERROR_CRC:
                    case ERROR_IO_DEVICE:
                        lvl = Level::Retry; typ = ErrType::Io; break;

                    default:
                        lvl = Level::Default; typ = ErrType::Default; break;
                }

                std::string msg = getSystemErrorMessageWindows(code, context);
                unsigned int t = tries.value_or(0);
                return Error(lvl, typ, t, std::move(msg));
            }
        private:
            Level lvl;
            ErrType errType;
            unsigned int retries;
            windowDesc affectedWindow;
    };

}

#endif