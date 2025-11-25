#ifndef DESKUPFILENOTCLEAREXCEPTION_H
#define DESKUPFILENOTCLEAREXCEPTION_H

#include <stdexcept>
#include <string>

namespace DeskUp::Exceptions{

	class FileNotClearException final : public std::runtime_error {
		public:
			FileNotClearException(const std::string& path);
			FileNotClearException(const std::string_view& path);
	};
}

#endif
