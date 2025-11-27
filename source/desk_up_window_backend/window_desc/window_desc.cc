#include "window_desc.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include <system_error>

namespace fs = std::filesystem;

windowDesc::windowDesc(){
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	pathToExec = "";
	name = "";
}

int windowDesc::saveTo(fs::path path){

    if(path.empty()){
        std::cerr << "SaveTo: error: file path empty" << std::endl;
        return ERR_EMPTY_PATH;
    }

    std::ofstream windowFile(path, std::ios::out);
    if(!windowFile.is_open()){

        std::error_code ec(errno, std::generic_category());
        std::cerr << "SaveTo: error opening file '" << path << "': " << ec.message() << std::endl;

        switch (errno) {
            case EACCES:
                return ERR_NO_PERMISSION;
            case ENOENT:
                return ERR_FILE_NOT_FOUND;
            case ENOSPC:
                return ERR_DISK_FULL;
            default:
                return ERR_FILE_NOT_OPEN;
        }
    }

    windowFile << this->pathToExec.string()
               << std::endl << this->x
               << std::endl << this->y
               << std::endl << this->w
               << std::endl << this->h;

    if(!windowFile.good()){
        return ERR_UNKNOWN;
    }

    windowFile.close();

    std::cout << "Saved window: " << std::endl
              << this->pathToExec << std::endl
              << this->x << " " << this->y << " " << this->w << " " << this->h << std::endl;

    return SAVE_SUCCESS;
}