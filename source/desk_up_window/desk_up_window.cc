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
        windows = current_window_backend.get()->getAllOpenWindows(current_window_backend.get());
    } catch(std::runtime_error &e){
        std::cout << e.what();
        return 0;
    } catch(...){
        /*TODO: Show GUI error indicating what happened.
        Here comes into play creating private exceptions 
        to indicate what happened, because depending on 
        the error, the action can be fatal, non-fatal or
        user dependent*/
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
            //TODO: show error message in GUI. Ask user if he wants to continue or not depending on the exception thrown
            std::cout << windows[i].name << " could not be saved to local!" << std::endl;
            continue;
        }
    }

    return 1;
}