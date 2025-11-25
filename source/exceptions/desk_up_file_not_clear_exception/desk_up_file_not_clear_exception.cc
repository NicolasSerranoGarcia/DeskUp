#include "exceptions/desk_up_file_not_clear_exception/desk_up_file_not_clear_exception.h"

using namespace DeskUp::Exceptions;

FileNotClearException::FileNotClearException(const std::string& path) :
	std::runtime_error(std::string("FileNotClearException: Could not clear file: ") + path) {}

	FileNotClearException::FileNotClearException(const std::string_view& path) :
	std::runtime_error(std::string("FileNotClearException: Could not clear file: ") + std::string(path)) {}