#include "desk_up_workspace_lookup_table.h"
#include "desk_up_dir.h"

#include <iostream>

using namespace DeskUp::Utils;

namespace DeskUp{
	LookUpTable& getWorkspaceDirTable(){
		using DeskUp::DESKUPDIR;
		static LookUpTable workspaceDirTable(fs::path(DESKUPDIR) / fs::path("workspaceTable.txt"));
		return workspaceDirTable;
	}
}