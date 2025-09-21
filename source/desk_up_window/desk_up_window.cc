#include "desk_up_window.h"

#include <vector>
#include <string>
#include <filesystem>

#include "window_global.h"

int DeskUpWindow::saveAllWindowsLocal(std::string workspaceName){
    
    std::string workspacePath = DESKUPDIR;
    workspacePath += "\\";
    workspacePath += workspaceName;
    std::filesystem::create_directory((std::filesystem::path) workspacePath);

    std::vector<windowDesc> windows;
    try{
        windows = current_window_backend->getAllWindows(current_window_backend);
    } catch(std::runtime_error &e){
        std::cout << e.what();
        return 0;
    } catch(...){
        std::cout << "Unexpected something";
        return 0;
    }

    for(unsigned int i = 0; i < windows.size(); i++){
        
        //create path
        std::string filePath(workspacePath);
        filePath += "\\";
        filePath += windows[i].name;


        std::cout << filePath << std::endl;
        
        if(!windows[i].saveTo(filePath)){
            std::cout << windows[i].name << " could not be saved to local";
            continue;
        }
    }

    return 1;
}   