#include "desk_up_frame.h"

#include "desk_up_window.h"
#include "window_core.h"

enum {
    ID_AddWorkspace = 1,
    ID_RestoreWorkspace,
};

DeskUpFrame::DeskUpFrame()
        : wxFrame(nullptr, wxID_ANY, "DeskUp")
{
    #ifdef WIN32
        SetIcon(wxICON(IDI_APPLICATION));
    #endif

    if(!DU_Init()){
        wxMessageBox("There was an error when initializing DeskUp. Try closing and opening the app again", "DeskUp error", wxSTAY_ON_TOP);
    }

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_AddWorkspace, "Add workspace \tctrl+N");
    menuFile->Append(ID_RestoreWorkspace, "Restore workspace \tctrl+R");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Workspace");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);
 
    CreateStatusBar();
    SetStatusText("Welcome to DeskUp!");

    Bind(wxEVT_MENU, &DeskUpFrame::OnAdd, this, ID_AddWorkspace);
    Bind(wxEVT_MENU, &DeskUpFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &DeskUpFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &DeskUpFrame::OnRestore, this, ID_RestoreWorkspace);
}

void DeskUpFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void DeskUpFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("About DeskUp message",
                 "About DeskUp", wxOK | wxICON_INFORMATION);
}

void DeskUpFrame::OnAdd(wxCommandEvent& event)
{
    wxString workspaceName;

    while(1){

        wxTextEntryDialog createWorkspace(this, "Enter the workspace name", "Add workspace");
        
        if(createWorkspace.ShowModal() != wxID_OK){
            return;
        } 

        workspaceName = createWorkspace.GetValue();

        if(workspaceName.empty()){
            wxMessageBox("The workspace name cannot be empty!", "Workspace name", wxOK | wxICON_ERROR);
            continue;
        }

        if(!DeskUpWindow::isWorkspaceValid(workspaceName.ToStdString())){
            wxMessageBox("The workspace name is not valid! Blacklisted characters: \\/:?*\"<>|", "Workspace name not valid", wxOK | wxICON_ERROR);
            continue;
        }

        break;

    }

    if (!DeskUpWindow::existsWorkspace(workspaceName.ToStdString())){
        DeskUpWindow::saveAllWindowsLocal(workspaceName.ToStdString());
    } else{
        wxMessageBox("The workspace already exists. Do you wish to rewrite it? ", "Workspace exists", wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxICON_QUESTION);
    }

}

void DeskUpFrame::OnRestore(wxCommandEvent& event)
{
        wxString workspaceName;

    while(1){

        wxTextEntryDialog createWorkspace(this, "Enter the workspace name", "Restore workspace");
        
        if(createWorkspace.ShowModal() != wxID_OK){
            return;
        } 

        workspaceName = createWorkspace.GetValue();

        if(workspaceName.empty()){
            wxMessageBox("The workspace name cannot be empty!", "Workspace name", wxOK | wxICON_ERROR);
            continue;
        }

        if(!DeskUpWindow::isWorkspaceValid(workspaceName.ToStdString())){
            wxMessageBox("The workspace name is not valid! Blacklisted characters: \\/:?*\"<>|", "Workspace name not valid", wxOK | wxICON_ERROR);
            continue;
        }

        break;

    }

    if (DeskUpWindow::existsWorkspace(workspaceName.ToStdString())){
        DeskUpWindow::restoreWindows(workspaceName.ToStdString());
    } else{
        wxMessageBox("The workspace does not exist!", "Workspace does not exist", wxOK | wxICON_INFORMATION);
    }
}
