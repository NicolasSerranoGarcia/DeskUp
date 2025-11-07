; -- DeskUp-installer.iss --

#define AppName "DeskUp"
#define RootDir "..\"
#define BuildDir "..\build"
#define InstallerIconFile "..\assets\DeskUp-installer.ico"
; expected to have run "windeployqt DeskUp.exe --release --dir ../build_inno"
#define BuildInno "..\build_inno"
#define ReadmeFile "..\README.md"
#define LicenseFile "..\LICENSE"
#define AppURL "https://DeskUp.github.io"
#define WorkspacesDir "{userappdata}\DeskUp" 
#define FileDesc = FileOpen("..\VERSION")
#define VersionNumber = FileRead(FileDesc)
#expr FileClose(FileDesc)
#define AppExeName "DeskUp.exe"
#define AppCopyright "© 2025 Nicolas Serrano Garcia"
#define AppDescription "DeskUp workspace manager"
#define AppCompany "DeskUp Project"

[Setup]
AppId={{A3D21D63-7C43-4F5D-8AF5-4A31BBF708CE}}
AppName={#AppName}
AppVersion={#VersionNumber}
AppVerName={#AppName} {#VersionNumber}
VersionInfoVersion={#VersionNumber}
VersionInfoCopyright={#AppCopyright}
VersionInfoCompany={#AppCompany}
VersionInfoDescription={#AppDescription}
AppPublisherURL={#AppURL}
WizardStyle=modern
;Program Files(x86)\DeskUp or Program Files\DeskUp, depending on the user arch
DefaultDirName={autopf}\{#AppName}
;In windows 10, the search bar included all files from A-Z. This is the name that appears as a group there
DefaultGroupName={#AppName}
;This is the name of the .exe installer
OutputBaseFilename={#AppName}-installer-{#VersionNumber}
;Where the Innosetup generated files will go
OutputDir={#BuildInno}\Output
LicenseFile={#LicenseFile}
Compression=lzma
SolidCompression=yes
SetupIconFile={#InstallerIconFile}
ShowLanguageDialog=no
UninstallDisplayName={#AppName}
AppModifyPath="{uninstallexe}"

[FILES]
; BuildInno contains the exe, dll and plugins needed
Source: "{#BuildInno}\*"; DestDir: "{app}";Flags: recursesubdirs; Excludes: "*.pdb;*.log;CMakeFiles\*"
Source: "{#ReadmeFile}"; DestDir: "{app}"; Flags: isreadme

[TASKS]
Name: startmenu; Description: "Add to Start Menu"; GroupDescription: "Integration"
Name: desktopicon; Description: "Create Desktop Shortcut"; GroupDescription: "Integration"
Name: showWorkspaces; Description: "Show the folder where the workspaces will be located"; GroupDescription: "Extras"

[Dirs]
Name: {#WorkspacesDir}

[ICONS]
Name: "{group}\DeskUp"; FileName: "{app}\{#AppExeName}"; Tasks: startmenu
Name: "{group}\{cm:ProgramOnTheWeb,DeskUp}"; FileName: "{#AppURL}"; Tasks: startmenu
Name: "{group}\{cm:UninstallProgram,DeskUp}"; FileName: "{uninstallexe}"; Tasks:startmenu
Name: "{group}\Workspaces"; FileName: "{#WorkspacesDir}"; Tasks: showWorkspaces

[RUN]
Filename: "{app}\{#AppExeName}"; WorkingDir: "{#WorkspacesDir}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: postinstall
Filename: "{#AppURL}"; Description: "Visit DeskUp website"; Flags: postinstall shellexec skipifsilent unchecked

[UninstallDelete]
Type: dirifempty; Name: "{#WorkspacesDir}"

; [Registry]
; Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\DeskUp.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\DeskUp.exe"
; Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\DeskUp.exe"; ValueType: string; ValueName: "Path"; ValueData: "{app}"

; Root: HKCR; Subkey: "*\shell\Open with DeskUp"; \
;     ValueType: string; ValueName: ""; ValueData: "Open with DeskUp"
; Root: HKCR; Subkey: "*\shell\Open with DeskUp\command"; \
;     ValueType: string; ValueName: ""; ValueData: """{app}\DeskUp.exe"" ""%1"""