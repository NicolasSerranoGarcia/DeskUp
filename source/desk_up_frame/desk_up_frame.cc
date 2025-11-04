#include "desk_up_frame.h"

#include <fstream>

#include <wx/aboutdlg.h>

#include "desk_up_error.h"
#include "desk_up_error_gui_converter.h"
#include "window_core.h"
#include "desk_up_window.h"


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

void DeskUpFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

void DeskUpFrame::OnAbout(wxCommandEvent&)
{


    wxAboutDialogInfo info;
    info.SetName("DeskUp");
    info.SetVersion(DESKUP_VERSION);
    info.SetDescription("A workspace manager for Windows that lets you save and restore window layouts.");
    info.SetCopyright("© 2025 Nicolas Serrano Garcia");
    info.SetDevelopers({"Nicolas Serrano Garcia"});
    info.SetLicence("Licensed under GPLv3. See LICENSE for details.");
    info.SetWebSite("https://github.com/NicolasSerranoGarcia/DeskUp");

    wxAboutBox(info);
}

void DeskUpFrame::OnAdd(wxCommandEvent&)
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
        
        if(auto saveRes =DeskUpWindow::saveAllWindowsLocal(workspaceName.ToStdString()); !saveRes.has_value()){
            DeskUp::UI::ErrorAdapter::showError(std::move(saveRes.error()));
        } else{
            showSaveSuccessful();
        }
    } else{
        int response = wxMessageBox("The workspace already exists. Do you wish to rewrite it? ", "Workspace exists",
                                                                                wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxICON_QUESTION);
        if(response == wxYES){
            DeskUpWindow::removeWorkspace(workspaceName.ToStdString());
            
            if(auto res = DeskUpWindow::saveAllWindowsLocal(workspaceName.ToStdString()); !res.has_value()){
                DeskUp::UI::ErrorAdapter::showError(std::move(res.error()));
            }
        }
    }

}

void DeskUpFrame::OnRestore(wxCommandEvent&)
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
        auto res = DeskUpWindow::restoreWindows(workspaceName.ToStdString());

        if(!res.has_value()){
            DeskUp::UI::ErrorAdapter::showError(std::move(res.error()));
        } else{
            showRestoreSuccessful();
        }
    } else{
        wxMessageBox("The workspace does not exist!", "Workspace does not exist", wxOK | wxICON_ERROR);
    }
}

int DeskUpFrame::showSaveSuccessful(){
    return wxMessageBox("The workspace was saved successfully!", "Workspace saved", wxOK | wxICON_DEFAULT_TYPE);
}

int DeskUpFrame::showRestoreSuccessful(){
    return wxMessageBox("The workspace was restored successfully!", "Workspace restored", wxOK | wxICON_DEFAULT_TYPE);
}