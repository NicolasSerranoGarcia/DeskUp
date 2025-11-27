#ifndef DESKUPDIR_H
#define DESKUPDIR_H

#include <string>

namespace DeskUp {
	// Singleton global holding the base DeskUp workspace directory.
	// Set during initialization and used across subsystems.
	extern std::string DESKUPDIR;
}

#endif
