#include "desk_up_backend_interface.h"

#include <vector>
#include <string>
#include <filesystem>
#include <cctype>

#include "window_core.h"
#include "desk_up_workspace_lookup_table.h"

namespace fs = std::filesystem;

//TODO: rewrite the error message to be the actual message you want shown, so as to be more specific with the message shown

static fs::path constructWsDir(std::string workspace){
	fs::path workspacePath = DESKUPDIR;
    workspacePath /= workspace;
	return workspacePath;
}

//if the dir already exists it just retuns the constructed path. Otherwise it creates it and returns the constructed path.
//This way we avoid errors coming from fs::create_directory
static fs::path createDirFromWs(std::string workspace){
	fs::path workspacePath = constructWsDir(workspace);
	if(fs::exists(workspacePath) && fs::is_directory(workspacePath)){
		return workspacePath;
	}

    fs::create_directory(workspacePath);
	return workspacePath;
}

DeskUp::Status DeskUpBackendInterface::saveAllWindowsLocal(std::string workspaceName){
	//it is mandatory that the files saved have w, h >= 0, 5 LINES, no endl

	auto& wsTable = DeskUp::getWorkspaceDirTable();

	fs::path workspacePath;

	try{

		//returns the path to the directory associated with the entry
		workspacePath = wsTable.addEntry(workspaceName);
	} catch(std::runtime_error& e){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Fatal, DeskUp::ErrType::Io, 0, "An error happened when trying to create the workspace. DeskUp will close"));
	}

	//get all the open windows
    auto windows = current_window_backend.get()->getAllOpenWindows(current_window_backend.get());

    if(!windows.has_value()){
        return std::unexpected(std::move(windows.error()));
    }

    DeskUp::Error lastErr;

	//used to assign different names to windows of the same instance
	int id = 0;

    for(unsigned int i = 0; i < windows.value().size(); i++){


		//make a copy of the workspaceDir
		fs::path copyWs(workspacePath);

		//add the file name
        copyWs /= windows.value()[i].name;

        if(existsFile(copyWs)){
            copyWs += std::to_string(id++);
        }

        if(int res = windows.value()[i].saveTo(copyWs); res < 0){

            auto err = DeskUp::Error::fromSaveError(res);

			//can only happen if the disk is full
            if(err.isFatal()){
                return std::unexpected(std::move(err));
            }

			//any other error is not fatal, so save the last one and continue
            lastErr = err;
        }
    }

	//if no error was set inside the loop, the error gets default initialized to no error
    if(!lastErr){
        return {};
    }

    return std::unexpected(std::move(lastErr));
}

DeskUp::Status DeskUpBackendInterface::restoreWindows(std::string workspaceName){
    //initially, the user will need to write the name of the workspace, but when it is shown as a choose option visually (select the workspace),
    //there will be no need to check if the workspace exists, because the same program will identify the name and therefore pass it correctly

	if(workspaceName.empty()){
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "restoreWindows|empty_path"));
	}

	auto& wsTable = DeskUp::getWorkspaceDirTable();

	fs::path p;
    try{
		p = wsTable.getDirFromEntry(workspaceName);
	}catch(std::runtime_error& e){
		//this just simply means there is an error in the workspace name itself and/or the deskup path
		return std::unexpected(DeskUp::Error(DeskUp::Level::Error, DeskUp::ErrType::InvalidInput, 0, "restoreWindows|no_path_" + p.string()));
	}

	//might want to ask the user
    bool forceTermination = true;

    for (const auto& file : fs::directory_iterator{p}) {
		//can't throw fatal errors
        auto res = current_window_backend->recoverSavedWindow(current_window_backend.get(), file.path());
        if (!res.has_value()){
			std::cout << "Unrecoverable window: " << res.error().what() << std::endl;
			return std::unexpected(std::move(res.error()));
		}

		//from here we expect valid window

		auto window = res.value();

        auto closeRes = current_window_backend->closeProcessFromPath(current_window_backend.get(), window.pathToExec, forceTermination);
        if (!closeRes.has_value() && closeRes.error().isFatal())
        if (!closeRes.has_value()){
            if(closeRes.error().isFatal()){
                return std::unexpected(std::move(closeRes.error()));
            }

            std::cout << "Unclosed window: " << closeRes.error().what() << std::endl;
        }

        auto loadRes = current_window_backend->loadWindowFromPath(current_window_backend.get(), window.pathToExec);
        if (!loadRes.has_value()){
            if(loadRes.error().isFatal()){
                return std::unexpected(std::move(loadRes.error()));
            }

            std::cout << "Unopened window: " << loadRes.error().what() << std::endl;
        }

        auto resizeRes = current_window_backend->resizeWindow(current_window_backend.get(), window);
        if (!resizeRes.has_value() && resizeRes.error().isFatal())
        if (!resizeRes.has_value()){
            if(resizeRes.error().isFatal()){
                return std::unexpected(std::move(resizeRes.error()));
            }

            std::cout << "Unresized window: " << resizeRes.error().what() << std::endl;
        }
    }

    return {};
};

bool DeskUpBackendInterface::isWorkspaceValid(const std::string& workspaceName){
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

bool DeskUpBackendInterface::existsWorkspace(const std::string& workspaceName){
    if(workspaceName.empty()){
        throw std::runtime_error("Workspace empty cannot be resolved to true or false");
    }

    auto& wsTable = DeskUp::getWorkspaceDirTable();

	try{
		//throws if it can't find the entry
		wsTable.getDirFromEntry(workspaceName);
	} catch(std::runtime_error& e){
		return false;
	}

    return true;
}

int DeskUpBackendInterface::removeWorkspace(const std::string& workspaceName){
    if(!existsWorkspace(workspaceName)){
        return 0;
    }

 	auto& wsTable = DeskUp::getWorkspaceDirTable();

	try{
		wsTable.deleteEntry(workspaceName);
	} catch(std::runtime_error& e){
		return 0;
	}


    return 1;
}

bool DeskUpBackendInterface::existsFile(const fs::path& filePath){
    if(filePath.empty()){
        return false;
    }

    if(!fs::exists(filePath) || !fs::is_regular_file(filePath)){
        return false;
    }

    return true;
}