#include "desk_up_window.h"

#include <vector>
#include <string>
#include <filesystem>

#include "window_core.h"

namespace fs = std::filesystem;

int DeskUpWindow::saveAllWindowsLocal(std::string workspaceName){
    
    std::string workspacePath = DESKUPDIR;
    workspacePath += "\\";
    workspacePath += workspaceName;
    fs::create_directory((fs::path) workspacePath);

    std::vector<windowDesc> windows;
    try{
        windows = current_window_backend.get()->getAllWindows(current_window_backend.get());
    } catch(std::runtime_error &e){
        std::cout << e.what();
        return 0;
    } catch(...){
        std::cout << "Something unexpected happened when saving all windows!" << std::endl;
        return 0;
    }

    for(unsigned int i = 0; i < windows.size(); i++){
        
        //create path
        std::string filePath(workspacePath);
        filePath += "\\";
        filePath += windows[i].name;


        std::cout << filePath << std::endl;
        
        if(!windows[i].saveTo(filePath)){
            //TODO: show error message in GUI. Ask user if he wants to continue or not
            std::cout << windows[i].name << " could not be saved to local!" << std::endl;
            continue;
        }
    }

    return 1;
}