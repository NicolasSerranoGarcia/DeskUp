#include "desk_up_window.h"

#include <vector>
#include <string>
#include <filesystem>
#include <cctype>

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

int DeskUpWindow::restoreWindows(std::string workspaceName){
    //initially, the user will need to write the name of the workspace, but when it is shown as a choose option visually (select the workspace), 
    //there will be no need to check if the workspace exists, because the same program will identify the name and therefore pass it correctly

    fs::path p (DESKUPDIR);
    p = p / workspaceName;

    std::error_code err;
    if(!fs::exists(p, err) || err){
        //TODO: throw an exception that represents that the workspace name does not exist
        return 0;
    }

    try{
        for(auto const &file : fs::directory_iterator{p}){
            windowDesc w = current_window_backend.get()->recoverSavedWindow(current_window_backend.get(), file.path());
            std::cout << w.x << " " << w.y << " " << w.w << " " << w.h << w.pathToExec << std::endl;
            current_window_backend.get()->closeWindowFromPath(current_window_backend.get(), w.pathToExec, true);
            current_window_backend.get()->loadWindowFromPath(current_window_backend.get(), w.pathToExec);
            current_window_backend.get()->resizeWindow(current_window_backend.get(), w);
        }
    //in the future, different exceptions must rethrow or do different things
    } catch(const fs::filesystem_error &e){
        std::cout << e.what() << std::endl;
    } catch(const std::runtime_error &r){
        std::cout << r.what() << std::endl;
    } catch(const std::invalid_argument &a){
        std::cout << a.what() << std::endl;
    } catch(const std::exception &ex){
        std::cout << ex.what() << std::endl;
    } catch(...){
        std::cout << "Something unexpected happened!" << std::endl;
        return 0;
    }

    return 1;
};

bool DeskUpWindow::isWorkspaceValid(const std::string& workspaceName){
    if(workspaceName.empty()){
        return false;
    }

    std::string blackList = {"\\/:?*\"<>|"};

    for(const char c : blackList){
        
        if(workspaceName.find(c) != std::string::npos){
            return false;
        }
    }

    return true;
}

bool DeskUpWindow::existsWorkspace(const std::string& workspaceName){
    if(workspaceName.empty()){
        return false;
    }

    fs::path p{DESKUPDIR};
    p /= workspaceName;

    if(!fs::exists(p) || !fs::is_directory(p)){
        return false;
    }

    return true;
}

int DeskUpWindow::deleteWorkspace(const std::string& workspaceName){
    if(!existsWorkspace(workspaceName)){
        return 0;
    }

    fs::path p{DESKUPDIR};
    p /= workspaceName;


    std::error_code err;

    uintmax_t n = fs::remove_all(p, err);

    if(err){
        return 0;
    }

    return n;
}