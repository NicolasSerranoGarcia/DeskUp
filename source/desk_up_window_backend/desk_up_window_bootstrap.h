/**
 * @file desk_up_window_bootstrap.h
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

#ifndef DESKUPWINDOWBOOSTRAP_H
#define DESKUPWINDOWBOOSTRAP_H

#include "desk_up_window_device.h"

/**
 * @struct DeskUpWindowBootStrap
 * @brief This struct is a wrapper that holds a call to create a window device.
 * 
 * @details Each backend packs all of it's functions in a device, which should be the only way to access it's calls. It fills backend-specific
 *          info in the device, as well as connecting all the implementations of the generic backend calls. 
 * 
 * 
 * @see DeskUpWindowDevice
 * @author Nicolas Serrano Garcia <serranogarcianicolas@gmail.com>
 * @version 0.1.0
 * @date 2025
 */
struct DeskUpWindowBootStrap{

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
     * @brief A pointer to function that creates a Device.
     *  
     * 
     * @return A pointer to a heap allocated windowDevice. 
     * @version 0.1.0
     * @date 2025
     */
    DeskUpWindowDevice (*createDevice)();

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