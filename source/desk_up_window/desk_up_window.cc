#include "desk_up_window.h"

#include <vector>
#include <string>
#include <filesystem>

#include <cctype>

#include "window_core.h"

namespace fs = std::filesystem;

DeskUp::Status DeskUpWindow::saveAllWindowsLocal(std::string workspaceName){
    
    fs::path workspacePath = DESKUPDIR;
    workspacePath /= workspaceName;
    fs::create_directory(workspacePath);
    
    auto windows = current_window_backend.get()->getAllOpenWindows(current_window_backend.get());

    if(!windows.has_value()){
        return std::unexpected(std::move(windows.error()));
    }

    DeskUp::Error lastErr;

    int id = 0;

    for(unsigned int i = 0; i < windows.value().size(); i++){
        
        //create path
        fs::path p(workspacePath);
        p /= windows.value()[i].name;

        if(existsFile(p)){
            p += std::to_string(id++);
        }
        
        if(int res = windows.value()[i].saveTo(p); res < 0){
            
            auto err = DeskUp::Error::fromSaveError(res);

            if(err.isFatal()){
                return std::unexpected(std::move(err));
            }

            lastErr = err;

            continue;
        }
    }

    if(lastErr.level() == DeskUp::Level::None){
        return {};
    }

    return std::unexpected(std::move(lastErr));
}

DeskUp::Status DeskUpWindow::restoreWindows(std::string workspaceName){
    //initially, the user will need to write the name of the workspace, but when it is shown as a choose option visually (select the workspace), 
    //there will be no need to check if the workspace exists, because the same program will identify the name and therefore pass it correctly

    fs::path p(DESKUPDIR);
    p /= workspaceName;

    if (!fs::exists(p) || !fs::is_directory(p)) {
        return std::unexpected(DeskUp::Error(
            DeskUp::Level::Fatal, DeskUp::ErrType::NotFound, 0,
            "restoreWindows: workspace not found: " + p.string()
        ));
    }

    bool forceTermination = true;
    for (const auto& file : fs::directory_iterator{p}) {
        auto res = current_window_backend->recoverSavedWindow(current_window_backend.get(), file.path());
        if (!res.has_value() && res.error().isFatal())
        if (!res.has_value()){
            if(res.error().isFatal()){
                return std::unexpected(std::move(res.error()));
            } 

            std::cout << "Unrecovered window: " << res.error().what();
        }

        auto closeRes = current_window_backend->closeProcessFromPath(current_window_backend.get(),
                                                                    res->pathToExec, forceTermination);
        if (!closeRes.has_value() && closeRes.error().isFatal())
        if (!closeRes.has_value()){
            if(closeRes.error().isFatal()){
                return std::unexpected(std::move(closeRes.error()));
            }

            std::cout << "Unclosed window: " << closeRes.error().what();
        }

        auto loadRes = current_window_backend->loadWindowFromPath(current_window_backend.get(), res->pathToExec);
        if (!loadRes.has_value()){
            if(loadRes.error().isFatal()){
                return std::unexpected(std::move(loadRes.error()));
            } 

            std::cout << "Unopened window: " << loadRes.error().what();
        }

        auto resizeRes = current_window_backend->resizeWindow(current_window_backend.get(), *res);
        if (!resizeRes.has_value() && resizeRes.error().isFatal()) 
        if (!resizeRes.has_value()){
            if(resizeRes.error().isFatal()){
                return std::unexpected(std::move(resizeRes.error()));
            }

            std::cout << "Unresized window: " << resizeRes.error().what();
        }
    }

    return {};
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

int DeskUpWindow::removeWorkspace(const std::string& workspaceName){
    if(!existsWorkspace(workspaceName)){
        return 0;
    }

    fs::path p{DESKUPDIR};
    p /= workspaceName;


    std::error_code err;

    fs::remove_all(p, err);

    if(err){
        return 0;
    }

    return 1;
}

bool DeskUpWindow::existsFile(const fs::path& filePath){
    if(filePath.empty()){
        return false;
    }

    if(!fs::exists(filePath) || !fs::is_regular_file(filePath)){
        return false;
    }

    return true;
}