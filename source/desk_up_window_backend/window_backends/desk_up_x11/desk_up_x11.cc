#include "desk_up_x11.h"

bool X11_isAvailable(DU_WindowDevice * device){
    //X11 returns nullptr when it cannot stablish a connection with the user display
    Display * display = ((windowData *) device->internalData)->display;

    return display != nullptr;
}

//this is a callback
int X11_errorHandlerNonFatal(Display * display, XErrorEvent * error){

    char * message;
    int messageLength = 500;

    XGetErrorText(error->display, error->error_code, message, messageLength);

    std::cout << "An error occurred in the display: " << error->display << "."
    << "Error: "<< message << "The involved resource: " << error->resourceid
    << std::endl;

    return 0;
}

//this is the callback for fatal errors
void X11_DisplayFatalMessage(Display *, void *){

    std::cout << "Connection with the server lost!" << std::endl;

    std::exit(1);
    return;
}

DU_WindowDevice * X11_CreateDevice(void){

    DU_WindowDevice * device = nullptr;
    
    device->getWindowHeight = X11_getWindowHeight;
    device->isAvailable = X11_isAvailable;
    
    windowData * data;
    data->display = XOpenDisplay(NULL);
    
    device->internalData = data;
    
    //set the error handler
    XSetErrorHandler(X11_errorHandlerNonFatal);
    XSetIOErrorExitHandler(data->display, X11_DisplayFatalMessage, (void *) 0);
    return device;
}


unsigned int X11_getWindowHeight(DU_WindowDevice * _this){

    windowData * data = (windowData *) _this->internalData;

    if(data->display == nullptr || !data->w){
        throw std::invalid_argument("Invalid arguments!");
    }

    XWindowAttributes * returnAttributes;

    //loads returnAttributes with height
    if(!XGetWindowAttributes(data->display, data->w, returnAttributes)){
        throw std::runtime_error("X11 could not return the window height!");
    }

    const unsigned int height = returnAttributes->height;

    return height;
}

unsigned int X11_getWindowWidth(DU_WindowDevice * _this){

    windowData * data = (windowData *) _this->internalData;

    if(data->display == nullptr || !data->w){
        throw std::invalid_argument("Invalid arguments!");
    }

    XWindowAttributes * returnAttributes;

    //loads returnAttributes with width
    if(!XGetWindowAttributes(data->display, data->w, returnAttributes)){
        throw std::runtime_error("X11 could not return the window width!");
    }

    const unsigned int width = returnAttributes->width;

    return width;
}

unsigned int X11_getWindowXPos(DU_WindowDevice * _this){

    windowData * data = (windowData *) _this->internalData;

    if(data->display == nullptr || !data->w){
        throw std::invalid_argument("Invalid arguments!");
    }

    XWindowAttributes * returnAttributes;

    //loads returnAttributes with x
    if(!XGetWindowAttributes(data->display, data->w, returnAttributes)){
        throw std::runtime_error("X11 could not return the window x position!");
    }

    const unsigned int x = returnAttributes->x;

    return x;
}

unsigned int X11_getWindowYPos(DU_WindowDevice * _this){

    windowData * data = (windowData *) _this->internalData;

    if(data->display == nullptr || !data->w){
        throw std::invalid_argument("Invalid arguments!");
    }

    XWindowAttributes * returnAttributes;

    //loads returnAttributes with y
    if(!XGetWindowAttributes(data->display, data->w, returnAttributes)){
        throw std::runtime_error("X11 could not return the window y position!");
    }

    const unsigned int y = returnAttributes->y;

    return y;
}

std::vector<windowDesc> X11_getAllWindows(DU_WindowDevice * _this){

    std::vector<windowDesc> windows;

    Xwindows

}

