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

   [X] create a simple diagram that explains the backend workflow using all the structs


## <1> 2. Use backend inside the project: files and connecting the backend with them

   [X] The program needs to save information about the workspace. To do this, DeskUp must save folders and files with the necessary information. Decide where this files and folders go. The way to go should be to leave them visible. the user can do whatever it wants with them. advice to not touch them if the user is not familiar with them. this way people have freedom over the use of the software

   [X] For the workspaces, a file for each window should be used. Each workspace will be a folder containing all the files of the window.

   [X] Decide the structure of the file itself: what goes inside? which pattern does it follow? (...)

   [X] The process of saving is pretty much straightforward, but loading workspaces needs an intermediate level. Find a good way to connect the backend with this information.

[X] Rename some files and folders incorrectly named: class DU_windowBootstrap to DeskUpWindowBootstrap, DU_WindowDevice to DeskUpWindowDevice... this keeps
    underscores (X11_) only for external API's or libraries

[X] choosing which device to use is not being done correctly. You need to implement it using cmake and defining macros.

[X] in x11 and win backends, getAllWindows functions need to set the name of the window

[X] Change the name of some incorrectly set classes and folders: windowDesc, DU_* Change everything DeskUp related to have DeskUpWhatever instead of DU_


# RESET - Linux dropped

## 1. Clean up save-workspace PR

[X] (optional for this task) if there are two windows of the same app, DeskUp won't save them correctly. Add some identifier like a number

[X] Clean up backend functions, document them and maybe change structure if something is not clear: missing desk_up_frame, desk_up and desk_up_window: desk_up_frame and desk_up may be volatile bc in the future DeskUp might migrate to Qt

[] (optional for this task) you may need to work with some apps individually. Some apps do not make it easy to get info from them. Chrome, for example, does not expose the tabs as windows or something the windows API can see. You will need to create or make user install an extension to get that info, and then redirect it to deskUp.

[X] the create Device function takes HWND. Instead, create a method to take a HWND and set the internal desk_up_hwnd to be the passed

[X] Make the current device be a smart pointer

[X] Add a readme inside the own backend to explain how the backend works

[X] notify in the documentation about the possible throws and errors of a backend function: missing desk_up_win

[X] Say in documentation of a function, which calls does it make to other functions: missing desk_up_win

[X] (Optional for this task) Create exceptions for things like: A window could not be saved because the path was changed; A window could not be saved because the program needs privileges...

[X] As you dropped linux, you need to clean the README, the github page and maybe the X11 backend

## 2. Implement loading workspaces - feature/load-workspace

[X] Implement a function in the backend to open an executable

[X] (optional for this task) If the user introduces a workspace name that already exists, show option to replace or not

[] (optional for this task) show visually all the workspaces available and let user choose one

[] (optional for this task) give an option to delete all the workspaces.

[] (optional for this task) add an option to run deskUp on windows startup

[] (optional for this task) give an option to clear all the tabs when opening a workspace or not.

[] (optional for this task) Some apps are linked with windows, because maybe the user downloaded them in the app store. This makes it so that they don't have an easy access when saving their info. Treat this in some way.(UWP)

[X] (optional for this task) look into github pages for the documentation, and link it to your main portfolio page

## <1> 3. Migrate to Qt - refactor/migrate-qt PR

[X] Migrate the frontend to qt

[] (optional for this task) fix the icon from qt

## <3> <1> 4. Implement CI/CD for windows again

[X] automate the workflow of development. In local, work with dynamic qt. Then, in the github workflows, setup the CD to pack everything into inno setup,
including the qt dlls.

[X] When creating the release, the executable does not get hooked. Te problem might be in the workspace where it gets put on (installer_artifacts)

[X] also, you need to see how to pack all the dependencies of the own qt dll libraries.

[X] <5> The CI workflow might need to be updated when you start generating the tests

[X] (optional for this task) include the option to download the pdf

[] (optional for this task) downloads page using javascript and fetching from the releases

##  5. Add testing for the backend and benchmarks for the app

[X] Create a dummy device which is used to check the deskUp backend. Note that it does not check the window device, but rather the structure associated with deskUp. Tests for each windowDevice function, with things like empty paths, negative dimensions, non valid pats... Try also looking into testing the device itself, specially windows.

[X] (optional for this task) Refactor the workflow of the backend operations. Make sure the errors are significant and that the functions d not fail unless there is a real error. Operations that retry should reflect it, consistency in param checking... With these changes, the already created tests should also be changed to reflect these changes, and they should adjust to the contracts of each function. It is important that the caller functions (generally the DeskUpBackendInterface::*) treat ALL of the errors that the backend specific functions can return.

[] (optional for this task) when deskup starts, check the privileges it was given, and show a message warning saying that deskup may not work properly
if it is not given the correct permissions

## 7. Create DeskUp Error system

[X] finish migrating the backend to use DeskUp:: namespace for errors

[X] cleanup, show messages in the frontend

[/] Restructure the project: create a library for each component, and create a correct namespace for each part of the project (DeskUp:: for globals, then create namespaces inside namespaces...). Also refactor the functions of the backend to take fs::path instead of std::string

[X] (optional for this task) mingw gets saved as the default bash terminal, fix it

[X] fix potential errors on the backend, document new code and do something with retry_op

[] (optional for this task) when launching explorer, instead of executing the full path to the explorer exe, just launch the path to the saved folder.
For this, you will need to save the path where the file explorer was when saving it, instead of the actual path to the executable

## 8. Support for browsers: Create an extension to gather information about the tabs and windows.

[] implement a pipeline with the extension that is able to recover the information of the tabs before saving the workspace.
The resulting information in the file saved should be marked as being a browser, which, tabs, windows, visibility... and then when loading it, it should do kind of the same thing as the desk up backend: The recovered information should be the same regardless of the browser, but
when executing it in the terminal, you should change the command depending on the browser.

## 9. refactor the GUI

[] using the schema photo, make a refactor, and implement the basic features planned

[] add workspace names and directories table correspondence. Data structure planned, and conversion in the interface

[] workspaces should have any name. For this, create a correspondence file between the workspace and the file itself

# MILESTONE 1.0.0 STABLE

## 1. Maintenance, cleanup, implementation of features, boost product...