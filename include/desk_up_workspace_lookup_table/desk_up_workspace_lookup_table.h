#ifndef DESKUPWORKSPACELOOKUPTABLE_H
#define DESKUPWORKSPACELOOKUPTABLE_H

#include "desk_up_lookup_table.h"

using namespace DeskUp::Utils;

namespace DeskUp{
	//DU_Init() must be called in order for this to work
	LookUpTable& getWorkspaceDirTable();
}

#endif