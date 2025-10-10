/**
 * @file backend_utils.h
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

#ifndef BACKENDUTILS_H
#define BACKENDUTILS_H

#include <Windows.h>
#include <string>


/**
 * @brief A function to convert from any wide char type to UTF-8. Used as a helper for getSystemErrorMessageWindows()
 * 
 * @param wideString A char chain of UTF-16 or other type of formatting 
 * @return \c std::string representing the string passed as a parameter converted to UTF-8
 * @version 0.1.0
 * @date 2025
 */
std::string WideStringToUTF8(LPCWCH wideString);

/**
 * @brief A function to convert a Windows error (the return of GetLastError() for example) to a readable error that can be printed
 * 
 * @param error The windows API error. You'll typically use getLastError() here
 * @param contextMessage An optional parameter to add context about the error. It will be appended at the begining of the string
 * @return \c std::string representing the formatted message and optionally a context error added at the begining
 * @version 0.1.0
 * @date 2025
 */
std::string getSystemErrorMessageWindows(DWORD error, const char contextMessage[] = ""); 

/**
 * @brief A function to convert a string to its lowercase version. Note that it does not alter the original string
 * 
 * @param s the string to convert to lowercase
 * @return \c std::string representing the lowercase version
 * @version 0.2.0
 * @date 2025
 */
std::string toLowerStr(const std::string& s);

/**
 * @brief A function to convert a generic path to a windows path using backslash path and later convert it to lowercase. Note that 
 * it does not alter the original string. 
 * 
 * @param s the string path to alter
 * @return \c std::string representing the altered path
 * @version 0.2.0
 * @date 2025
 */
std::string normalizePathLower(const std::string& p);

/**
 * @brief A function to convert a normal string into a wide one. Note that the original string doesn't get altered
 * 
 * @param s the string intended to convert
 * @return \c std::wstring representing the wide string
 * @version 0.2.0
 * @date 2025
 */
std::wstring UTF8ToWide(const std::string& s);

#endif