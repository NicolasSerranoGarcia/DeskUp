#include "window_desc.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int windowDesc::saveTo(fs::path path){
    
    if(path.empty()){
        std::cout << "file path empty" << std::endl;
        return 0;
    }
    
    std::ofstream windowFile;

    windowFile.open(path, std::ios::out);

    if(!windowFile.is_open()){
        std::cout << "SaveTo: open: Could not open file" << path << std::endl;
        return 0;
    }


    windowFile << this->pathToExec 
    << std::endl << this->x 
    << std::endl << this->y 
    << std::endl << this->w 
    << std::endl << this->h;

    std::cout << this->pathToExec 
    << std::endl << this->x 
    << std::endl << this->y 
    << std::endl << this->w 
    << std::endl << this->h << "helloue" << std::endl;

    windowFile.close();

    return 1;
}