#include "window_desc.h"

#include <fstream>
#include <iostream>

int windowDesc::saveTo(std::string path){
    
    if(path.empty()){
        std::cout << "file path empty" << std::endl;
        return 0;
    }
    
    std::ofstream windowFile;

    windowFile.open(path, std::ios::out);

    if(!windowFile.is_open()){
        return 0;
    }


    windowFile << this->pathToExec 
    << std::endl << this->x 
    << std::endl << this->y 
    << std::endl << this->w 
    << std::endl << this->h;

    windowFile.close();

    return 1;
}