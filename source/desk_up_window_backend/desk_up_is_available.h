/**
 * @file desk_up_is_available.h
 * @brief A struct wrapper used in DeskUp backend
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

#ifndef DESKUPISAVAILABLE_H
#define DESKUPISAVAILABLE_H

/**
 * @struct DeskUpIsAvailable
 * @brief This struct is a wrapper for a call to a window backend function that checks whether if that backend is available in this system.
 * 
 * @details This call checks if the backend device it comes from is available on this system.
 * 
 * Each backend will have an instance of this struct, called \c <backendName>IsAvailable, that window_core.h file will use inside
 * DU_Init()
 * 
 * 
 * @see DeskUpWindowDevice
 * @author Nicolas Serrano Garcia <serranogarcianicolas@gmail.com>
 * @version 0.1.0
 * @date 2025
 */
struct DeskUpisAvailable{
    /**
     * @brief The name of the backend associated with the device call.
     * 
     * @warning This is a raw C-string pointer. The referenced literal
     *          becomes invalid once the struct goes out of scope.
     * 
     * @version 0.1.0
     * @date 2025 
     */
    const char * name;

    /**
     * A pointer to function that is used to check whether if a given backend is available in the current device. 
     * 
     * @return \c true if the backend is available in this device, \c false otherwise
     * @version 0.1.0
     * @date 2025
     */
    bool (*isAvailable)(void);
};

#endif