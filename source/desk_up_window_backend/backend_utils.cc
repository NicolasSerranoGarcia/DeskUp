#include "backend_utils.h"

#include <stdlib.h>


std::string getSystemErrorMessageWindows(DWORD error, const char contextMessage[]) {
    if (!error) return "unknown error passed as parameter!";

    LPWSTR messageBuffer = nullptr;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    
    DWORD charsWritten = FormatMessageW(
        flags,
        nullptr,
        error,
        0,                  // Language ID
        reinterpret_cast<LPWSTR>(&messageBuffer),
        0,
        nullptr
    );

    std::string finalMessage;

    if (charsWritten) {
        // Convert from wide-char to UTF-8
        std::string utf8Buffer = WideStringToUTF8(messageBuffer);
        if (!utf8Buffer.empty()) {
            finalMessage = contextMessage;
            finalMessage += utf8Buffer;
        }
        LocalFree(messageBuffer);
    } else {
        finalMessage = contextMessage;
        finalMessage += "Unknown Windows error.";
    }

    return finalMessage;
}


std::string WideStringToUTF8(LPWSTR wideString) {
    if (!wideString) return "";

    // Input for size calculation
    UINT codePage = CP_UTF8;                 // Convert to UTF-8
    DWORD conversionFlags = 0;               // No special flags
    LPCWCH sourceWideString = wideString;   // Input UTF-16 string
    int inputCharacterCount = -1;            // Null-terminated
    LPSTR destinationBuffer = nullptr;       // No buffer yet, we only calculate size
    int destinationBufferSize = 0;           // Buffer size = 0
    LPCCH defaultChar = nullptr;             // Not used
    LPBOOL usedDefaultChar = nullptr;        // Not used

    // Get required buffer size for UTF-8 string (including null terminator)
    int utf8ByteCount = WideCharToMultiByte(
        codePage,
        conversionFlags,
        sourceWideString,
        inputCharacterCount,
        destinationBuffer,
        destinationBufferSize,
        defaultChar,
        usedDefaultChar
    );

    if (utf8ByteCount <= 0) return "";

    // Allocate string to hold the UTF-8 result
    std::string utf8String(utf8ByteCount - 1, 0); // exclude null terminator

    // Set up the parameters again for the actual conversion
    destinationBuffer = utf8String.data();
    destinationBufferSize = utf8ByteCount;

    // Perform the conversion
    int convertedBytes = WideCharToMultiByte(
        codePage,
        conversionFlags,
        sourceWideString,
        inputCharacterCount,
        destinationBuffer,
        destinationBufferSize,
        defaultChar,
        usedDefaultChar
    );

    if (convertedBytes <= 0) return "";

    return utf8String;
}