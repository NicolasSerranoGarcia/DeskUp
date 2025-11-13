#include "backend_utils.h"

#include <string>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>

std::string WideStringToUTF8(LPCWCH wideString) {
    if (!wideString) return {};

    // First call: ask Windows how many bytes are needed for UTF-8 conversion (including null terminator).
    int needed = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return {};

    // Allocate std::string with enough space for UTF-8 result.
    std::string utf8(needed, '\0');

    // Second call: actually convert the UTF-16 wide string to UTF-8.
    int written = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, utf8.data(), needed, nullptr, nullptr);
    if (written <= 0) return {};

    // Remove the null terminator because std::string manages its own.
    if (!utf8.empty() && utf8.back() == '\0') utf8.pop_back();
    return utf8;
}

std::string getSystemErrorMessageWindows(DWORD error, const std::string_view& contextMessage) {
    if (!error) return "unknown error passed as parameter!";

    LPWSTR messageBuffer = nullptr;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                | FORMAT_MESSAGE_FROM_SYSTEM
                | FORMAT_MESSAGE_IGNORE_INSERTS;

    // Call Windows API to translate an error code into a human-readable wide string.
    // Input: error code (DWORD), flags, system language.
    // Output: messageBuffer points to allocated wide string, charsWritten = number of wide chars.
    DWORD charsWritten = FormatMessageW(
        flags,
        nullptr,
        error,
        0,
        reinterpret_cast<LPWSTR>(&messageBuffer),
        0,
        nullptr
    );

    std::string finalMessage;

    finalMessage = contextMessage;
    if (charsWritten && messageBuffer) {
        // Copy the wide string into std::wstring and trim trailing CR/LF.
        std::wstring wmsg(messageBuffer, charsWritten);

        // Convert the system wide string to UTF-8 std::string.
        std::string utf8 = WideStringToUTF8(wmsg.c_str());

        // Free the buffer allocated by FormatMessageW.
        LocalFree(messageBuffer);

        // Build the final message string: context text + translated system error.
        finalMessage += utf8.empty() ? "|no_error" : (std::string)("|") + utf8;
    } else {
        // If FormatMessageW failed, fall back to a generic message.
        finalMessage += "|no_error";
    }

    return finalMessage;
}

std::wstring UTF8ToWide(const std::string& s){
    if (s.empty()){
        return L"";
    }

    //get the necessary size that the conversion will need
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    //create a string with that size to assure capacity
    std::wstring w(n, L'\0');
    //make the conversion
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), n);
    return w;
}

#endif

std::string toLowerStr(const std::string& s){

    std::string res;
    res.reserve(s.size());
    for (unsigned char c : s) {
        res += static_cast<char>(std::tolower(c));
    }
    return res;
}

std::string normalizePathLower(const std::string& p){
    std::string s = p;
    std::replace(s.begin(), s.end(), '/', '\\');
    return toLowerStr(s);
}