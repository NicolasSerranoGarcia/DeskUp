#ifndef WINDOWDESC_H
#define WINDOWDESC_H

#include <string>

// this is an abstract struct that saves all the info needed to restore a window to its original state  
typedef struct windowDesc{
    char * name;
    int x,y,w,h;
    std::string pathToExec;

    int saveTo(std::string path);

    bool operator!() const {
        return !x && !y && !w && !h;
    }
};

#endif