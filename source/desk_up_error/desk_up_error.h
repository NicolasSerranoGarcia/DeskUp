/**
 * @file desk_up_error.h
 * @brief The declaration of the error system of Desk Up
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

#ifndef DESKUPERROR_H
#define DESKUPERROR_H

#include <string>
#include <stdexcept>
#include <optional>
#include <expected>

#include <Windows.h>

#include "window_desc.h"
#include "backend_utils.h"

namespace DeskUp {

    enum class Level {
        Fatal,
        Error,
        Warning,
        Retry,
        Info,
        Debug,
        Default,
        None
    };

    enum class ErrType {
        InsufficientMemory,
        AccessDenied,
        SharingViolation,
        Io,
        NotFound,
        DiskFull,
        DeviceNotFound,
        Timeout,
        ResourceBusy,
        FileNotFound,
        InvalidFormat,
        InvalidInput,
        CorruptedData,
        OutOfRange,
        NetworkError,
        ConnectionRefused,
        HostUnreachable,
        ProtocolError,
        Unexpected,
        NotImplemented,
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
            static Error fromLastWinError(DWORD error, std::string_view context = "", std::optional<unsigned int> tries = std::nullopt);

            static Error fromLastWinError(std::string_view context = "", std::optional<unsigned int> tries = std::nullopt);

            //uses SaveErrorCode, defined in window_desc.cc
            static Error fromSaveError(int e);
        private:
            Level lvl;
            ErrType errType;
            unsigned int retries;
            windowDesc affectedWindow;
    };

    template <typename T>
    using Result = std::expected<T, DeskUp::Error>;
    using Status = std::expected<void, DeskUp::Error>;
}

#endif