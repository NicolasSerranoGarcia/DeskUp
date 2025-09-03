#ifndef BACKENDUTILS_H
#define BACKENDUTILS_H

#include <Windows.h>

//converts an error of windows into char. Facilitates error handling
const char * getSystemErrorMessageWindows(DWORD error, const char contextMessage[] = "Error: ");

#endif