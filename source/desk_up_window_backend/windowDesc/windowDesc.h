#ifndef WINDOWDESC_H
#define WINDOWDESC_H

#include <filesystem>

// this is an abstract struct that saves all the info needed to restore a window to its original state  
typedef struct windowDesc{
    int x,y,w,h;
    std::filesystem::path pathToExec;
};

#endif