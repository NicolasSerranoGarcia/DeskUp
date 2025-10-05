/**
 * @file desk_up_window_device.h
 * @brief The struct descriptor of a window backend
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

#ifndef DESKUPWINDOWDEVICE_H
#define DESKUPWINDOWDEVICE_H

#include <vector>
#include <string>

#include "window_desc.h"

/**
 * @struct DeskUpWindowDevice
 * @brief This abstract struct represents all the common calls that any backend must have.
 * 
 * @details  There are multiple pointers to functions inside this struct. Each one of them gets connected to a backend function whenever
 *           the backend gets created. When invoking the pointer, it implicitly calls the correct backend function, thus giving the
 *           expected result for that specific backend.
 *           This struct also carries specific backend information needed to get information from a backend successfully. 
 *           
 * 
 * @see windowData
 * @author Nicolas Serrano Garcia <serranogarcianicolas@gmail.com>
 * @version 0.1.0
 * @date 2025
 */
struct DeskUpWindowDevice{

    /**
     * @brief A pointer to function that is used to get the height of a window.
     * 
     * @param _this The very same instance
     * @return An \c unsigned \c int representing the height of the window
     * @version 0.1.0
     * @date 2025
     */
    unsigned int (*getWindowHeight)(DeskUpWindowDevice * _this);

    /**
     * @brief A pointer to function that is used to get the width of a window.
     * 
     * @param _this The very same instance
     * @return An \c unsigned \c int representing the width of the window
     * @version 0.1.0
     * @date 2025
     */
    unsigned int (*getWindowWidth)(DeskUpWindowDevice * _this);

    /**
     * @brief A pointer to function that is used to get the X position of a the top left corner of a window.
     * 
     * @param _this The very same instance
     * @return An \c int representing the X position of the top left corner of a window
     * @version 0.1.0
     * @date 2025
     */
    int (*getWindowXPos)(DeskUpWindowDevice * _this);

    /**
     * @brief A pointer to function that is used to get the Y position of a the top left corner of a window.
     * 
     * @param _this The very same instance
     * @return An \c int representing the Y position of the top left corner of a window
     * @version 0.1.0
     * @date 2025
     */
    int (*getWindowYPos)(DeskUpWindowDevice * _this);

    /**
     * @brief A pointer to function that is used to get the generic DeskUp workspaces path.
     * 
     * @details The return path is the path to the top-level folder where all the user workspaces are saved. A workspace
     * whose name is "foo" will have it's information saved in \c getDeskUpPath() \c + \c "/foo"
     * 
     * @return An \c std::string representing the path to the top-level Desk Up workspaces folder
     * @version 0.1.0
     * @date 2025
     */
    std::string (*getDeskUpPath)(void);
    
    /**
     * @brief A pointer to function that is used to get a list of abstract windows. For any backend to return the same thing, 
     * an abstract struct representing a windows is created. Regardless of the implementation, every device returns a vector of this type.
     * 
     * @param _this The very same instance
     * @return An \c std::vector representing all the visible and non-minimized windows currently open
     * @version 0.1.0
     * @date 2025
     */
    std::vector<windowDesc> (*getAllWindows)(DeskUpWindowDevice * _this);

    /**
     * @brief A pointer to function that is used to open a window from a given path. If the path is empty, 
     * 
     * @param _this The very same instance
     * @param path a \c const \c char* to the executable
     * @return \c void
     * @version 0.1.0
     * @date 2025
     */
    void (*loadProcessFromPath)(DeskUpWindowDevice * _this, const char * path);

    /**
     * @brief A pointer that points to the specific information needed by each backend
     * 
     * @details each backend defines WindowData, which is the template to seek the values of this pointer. Whenever a device call wants to access
     *          backend-specific info, this pointer gets casted to \c windowData* by \c std::reinterpret_cast. Then the backend accesses whatever
     *          information it needs. Finally, the same call casts back this pointer to \c void* , leaving everything as it was. 
     *          One can interpret this pointer as a "black box", or a "pouch", where the info gets thrown inside, and then reinterpreted back whenever
     *          it is needed
     * 
     * @version 0.1.0
     * @date 2025
     */
    void * internalData;
};

#endif