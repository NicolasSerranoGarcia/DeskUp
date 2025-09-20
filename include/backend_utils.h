#ifndef BACKENDUTILS_H
#define BACKENDUTILS_H

#include <Windows.h>
#include <string>
    
//converts an error of windows into char. Facilitates error handling
std::string getSystemErrorMessageWindows(DWORD error, const char contextMessage[] = "Error: "); 

std::string WideStringToUTF8(LPWSTR wideString);

#endif