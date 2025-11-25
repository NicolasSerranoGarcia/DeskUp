#include "exceptions/desk_up_file_not_open_exception/desk_up_file_not_open_exception.h"

using namespace DeskUp::Exceptions;

FileNotOpenException::FileNotOpenException(const std::string& path) :
	std::runtime_error(std::string("FileNotOpenException: Could not clear file: ") + path) {}

FileNotOpenException::FileNotOpenException(const std::string_view& path) :
	std::runtime_error(std::string("FileNotOpenException: Could not clear file: ") + std::string(path)) {}