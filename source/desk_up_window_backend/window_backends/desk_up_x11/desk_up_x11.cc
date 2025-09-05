#include "desk_up_x11.h"

#include <X11/Xatom.h>
#include <unistd.h>
#include <string>

bool X11_isAvailable(DU_WindowDevice * device){
    
    if(!device) return false;
    
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

    // in case the connection is not successful, we return the device early
    // with the null display so that X11_isAvailable() returns false.
    // If we didn't return early, the subsequent calls to xlib inside this function that take
    // the display as a parameter will return an error because the display is not available.
    // in terms of functioning, it won't matter, because this function is only called once and 
    // right before calling X11_isAvailable(), so even though we would
    if(!data->display){
        device->internalData = data;
        return device;
    }

    //if we didn't return early this would give an error. It wont affect the overall functioning, but we avoid
    //getting an error here 
    Screen * defaultScreen = XDefaultScreenOfDisplay(data->display);
    
    data->screen = defaultScreen;
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
    if(XGetWindowAttributes(data->display, data->w, returnAttributes) != Success){
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
    if(XGetWindowAttributes(data->display, data->w, returnAttributes) != Success){
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
    if(XGetWindowAttributes(data->display, data->w, returnAttributes) != Success){
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
    if(XGetWindowAttributes(data->display, data->w, returnAttributes) != Success){
        throw std::runtime_error("X11 could not return the window y position!");
    }

    const unsigned int y = returnAttributes->y;

    return y;
}

char * X11_getPathFromWindow(DU_WindowDevice * _this){
    
    windowData * data = (windowData *)_this->internalData;

    Display * display = data->display;

    Window window = data->w;



    Atom pidAtom = XInternAtom(display, "_NET_WM_PID", false);

    if(pidAtom == None) return "";

    unsigned long offset = 0;
    //specifies the number of elements to read
    unsigned long readNum = 1;
    //specifies if the property gets deleted when reading it
    bool noDelete = false;

    Atom * actualType;

    int actualFormat;

    unsigned long nitems, bytesAfter;

    unsigned char *prop = nullptr;

    const Status s = XGetWindowProperty(display, window, pidAtom, offset, readNum, noDelete, /*expected typeof _NET_WM_PID*/XA_CARDINAL, actualType, &actualFormat, 
        &nitems, &bytesAfter, &prop);

        //conditions that meet when the atom does not exist, which tels us that whoever created the application that launches the window 
        //did not put the PID as a property
    if(s != Success && (actualType == None) && !actualFormat && !bytesAfter){
            throw std::runtime_error("X11 could not get the atom associated with the PID. Either the property does not exist or there was a problem accessing it");
    } else if (s != Success){
        throw std::runtime_error("Something unexpected happened when trying to access the window PID");
    }

    pid_t pid = *(pid_t *)prop;

    /*access the process with the pid*/
    std::string symPath = "/proc/";
    symPath += pid;
    symPath += "/exe";

    char buff[1024];

    const ssize_t size = readlink(symPath.c_str(), buff, sizeof(buff) - 1);

    if(size == -1){
        throw std::runtime_error("readlink could not convert the pid into the executable path");
    }

    return buff;
}

windowDesc X11_getWindowDescFromWindow(DU_WindowDevice * _this){
    windowDesc newWindow;

    try{
        newWindow.x = X11_getWindowXPos(_this);
        newWindow.y = X11_getWindowXPos(_this);
        newWindow.w = X11_getWindowXPos(_this);
        newWindow.h = X11_getWindowXPos(_this);
        newWindow.pathToExec = std::string(X11_getPathFromWindow(_this));
    } catch(std::invalid_argument e){
        std::cout << e.what() << std::endl;
        return (windowDesc) {0,0,0,0,""};
    } catch(std::runtime_error e){
        std::cout << e.what() << std::endl;
        return (windowDesc) {0,0,0,0,""};
    }

    return newWindow;
}

std::vector<windowDesc> X11_getAllWindows(DU_WindowDevice * _this){

    windowData * data = ((windowData *)_this->internalData);

    Screen * screen = data->screen;

    Display * display = data->display;

    std::vector<windowDesc> windows;

    Window rootWindow = XRootWindowOfScreen(screen);

    Window ** childWindows;

    unsigned int * numChildWindows;

    //loads our variables with the info
    if(XQueryTree(display, rootWindow, /*root window, we dont care*/ 0, /*parent window, we dont care*/ 0, childWindows, numChildWindows) != Success){
        throw std::runtime_error("X11 could not return the childs of the root window!");
    }

    std::vector<windowDesc> windows;

    for(unsigned int i = 0; i < *numChildWindows; i++){
        //load the window inside the windowDevice
        data->w = *childWindows[i];
        const windowDesc newWindow = X11_getWindowDescFromWindow(_this);
        if(!newWindow){
            throw std::runtime_error("X11 could not get all the windows information!");
        }

        windows.push_back(newWindow);
    }

    return windows;
}

