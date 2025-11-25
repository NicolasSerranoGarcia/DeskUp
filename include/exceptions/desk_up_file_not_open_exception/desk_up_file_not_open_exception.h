#ifndef DESKUPFILENOTOPENEXCEPTION_H
#define DESKUPFILENOTOPENEXCEPTION_H

#include <stdexcept>
#include <string>

namespace DeskUp::Exceptions{

	class FileNotOpenException final : public std::runtime_error {
		public:
			FileNotOpenException(const std::string& path);
			FileNotOpenException(const std::string_view& path);
	};
}

#endif
