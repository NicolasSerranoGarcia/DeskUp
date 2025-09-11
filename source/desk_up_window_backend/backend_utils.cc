#include "backend_utils.h"

#include <stdlib.h>
#include <string>


const char * getSystemErrorMessageWindows(DWORD error, const char contextMessage[]){
    
    if(!error){
        return "unknown error passed as parameter!";
    }
    
    LPWSTR message;

    auto errorType = FORMAT_MESSAGE_FROM_SYSTEM;
    auto returnLang = LANG_SYSTEM_DEFAULT;
    int returnSize = 500;
    va_list * args = nullptr;

    FormatMessage(errorType, 0, error, returnLang, message, returnSize, args);

    //convert from wanky windows type to char
    char buffer[returnSize];

    wcstombs(buffer, message, returnSize);

    std::string errMessage = contextMessage;
    errMessage += buffer;

    return errMessage.c_str();
}