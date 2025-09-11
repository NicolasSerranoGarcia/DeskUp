#include "desk_up_window.h"

#include <vector>

#include "window_global.h"

int DeskUpWindow::saveAllWindowsLocal(std::string workspaceName){
    
    std::string workspacePath = DESKUPDIR;
    workspacePath += "/";
    workspacePath += workspaceName;

    std::vector<windowDesc> windows = current_window_backend->getAllWindows(current_window_backend);

    for(unsigned int i = 0; i < windows.size(); i++){
        
        //create path
        std::string filePath(workspacePath);
        filePath += "/";
        filePath += windows[i].name;
        
        if(!windows[i].saveTo(filePath)){
            //if any fails, the action gets aborted and user will be notified in the caller
            return 0;
        }
    }

    return 1;
}   