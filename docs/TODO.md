> Conventions:
> \# is used for BIG stages: going from MVP to stable should have a change in title.
> \#\# is used for tasks. tasks can carry an arbitrary number of chores, marked with []. chores can also appear inside a single # title. 
> tasks that depend on other tasks being completed are marked with a <previous-task> tag at the beginning of the title


# Start - Setup tools

[] Add previous documentation of Cmake-test for information about flags

[X] Add support for tests using the CMake-test ones.  

[X] Adapt workflows from CMake-test to work: needs to download wxWidgets

[X] Add support for benchmark using the CMake-test ones

[X] Fix workflows to make them compatible when they run, When run on linux to develop and when run on windows to develop

[X] Pack Linux as an AppImage

[X] Create icon images with distinct sizes

[X] AppImage does not open when double clicking it 

[] (Later in project) Add metadata to Appimage:
    [] The template for the file is already inside appImage.cmake -> sign the appImage, add screenshots...
        https://docs.appimage.org/packaging-guide/optional/signatures.html
        https://www.gnupg.org/gph/es/manual/x154.html
    [] When you enter a stable version (1.x.x) replace the string inside appImage.cmake called PROJECT_VERSION_SHORT inside the xml metadata file

[] (Later in project) Distribute files correctly insdie AppDir:
    [] the structure inside the AppDir should mirror the one on the system, even though the app will go to the system files


[X] add wxwidget libraries automatically to appimage when building

[/] .desktop has lots of functionalities. Look at usr/share/applications/libreoffice-math.desktop

[] Add a dialog that shows up when pressing uninstall on linux. For it, uninstall.sh should call an executable that uses wxWidgets to show a dialogue that asks to uninstall

[] Optional: Add more actions in the linux .desktop for things like Add Workspace or Load Workspace

[] when user installs a copy again, resolve the conflicts with the previous install --IMPORTANT
    - Problems associated with this: Version control for end users: What happens when a user downloads an AppImage that has a newer version than the old one? What happens on the opposite case? Solution may e to create a built-in package manager. Installing and uninstalling already works. The problem comes when user downloads multiple releases or old ones...

    [] for this: create dialogs that pop up when trying to install a release that's already installed. When downloading differing versions, ask the user what to do

[X] maybe take out .tgz from the releases

# MVP

## 1. Windowing backend structure:

   [X] X11 and WIN32 API need to be used inside the project. 
   
   [X] File/Folder structure for support of the backend needs to be added, for CMake and for the code itself.
   
   [] create a simple diagram that explains the backend workflow using all the structs


## <1> 2. Use backend inside the project: files and connecting the backend with them

   [] The program needs to save information about the workspace. To do this, DeskUp must save folders and files with the necessary information. Decide where this files and folders go. The way to go should be to leave them visible. the user can do whatever it wants with them. advice to not touch them if the user is not familiar with them. this way people have freedom over the use of the software

   [X] For the workspaces, a file for each window should be used. Each workspace will be a folder containing all the files of the window.

   [X] Decide the structure of the file itself: what goes inside? which pattern does it follow? (...)

   [] The process of saving is pretty much straightforward, but loading workspaces needs an intermediate level. Find a good way to connect the backend with this information.

[] Rename some files and folders incorrectly named: class DU_windowBootstrap to DeskUpWindowBootstrap, DU_WindowDevice to DeskUpWindowDevice... this keeps
    underscores (X11_) only for external API's or libraries

[X] choosing which device to use is not being done correctly. You need to implement it using cmake and defining macros.

[X] in x11 and win backends, getAllWindows functions need to set the name of the window

[] Change the name of some incorrectly set classes and folders: windowDesc, DU_* Change everything DeskUp related to have DeskUpWhatever instead of DU_


# RESET - Linux dropped

## 1. Clean up save-workspace PR

[] (optional for this task) if there are two windows of the same app, DeskUp won't save them correctly. Add some identifier like a number

[X] Clean up backend functions, document them and maybe change structure if something is not clear: missing desk_up_frame, desk_up and desk_up_window: desk_up_frame and desk_up may be volatile bc in the future DeskUp might migrate to Qt

[] (optional for this task) you may need to work with some apps individually. Some apps do not make it easy to get info from them. Chrome, for example, does not expose the tabs as windows or something the windows API can see. You will need to create or make user install an extension to get that info, and then redirect it to deskUp.

[X] the create Device function takes HWND. Instead, create a method to take a HWND and set the internal desk_up_hwnd to be the passed

[X] Make the current device be a smart pointer

[X] Add a readme inside the own backend to explain how the backend works

[X] notify in the documentation about the possible throws and errors of a backend function: missing desk_up_win

[/] Say in documentation of a function, which calls does it make to other functions: missing desk_up_win

[] (Optional for this task) Create exceptions for things like: A window could not be saved because the path was changed; A window could not be saved because the program needs privileges...

[X] As you dropped linux, you need to clean the README, the github page and maybe the X11 backend

## 2. Implement loading workspaces - feature/load-workspace

[X] Implement a function in the backend to open an executable

[] (optional for this task) If the user introduces a workspace name that already exists, show option to replace or not

[] show visually all the workspaces available and let user choose one

[] give an option to delete all the workspaces. 

[] add an option to run deskUp on windows startup 

[] when saving the windows, also save the z-component of the window

## <1> 3. Migrate to Qt - refactor/migrate-qt PR

## <3> <1> 4. Implement CI/CD for windows again

##  5. Add testing for the backend and benchmarks for the app

[] Create a dummy device which is used to check the deskUp backend. Note that it does not check the window device, but rather the structure associated with deskUp. Tests for each windowDevice function, with things like empty paths, negative dimensions, non valid pats...