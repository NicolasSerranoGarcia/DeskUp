#include "desk_up_frame.h"

#include "desk_up_window.h"
#include "window_global.h"

DeskUpFrame::DeskUpFrame()
        : wxFrame(nullptr, wxID_ANY, "DeskUp")
{
    #ifdef WIN32
        SetIcon(wxICON(IDI_APPLICATION));
    #endif

    if(!DU_Init((HWND) this->GetHandle())){
        wxMessageBox("There was an error when initializing DeskUp. Try closing and opening the app again", "DeskUp error", wxSTAY_ON_TOP);
    }

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_ADD, "Add workspace \tctrl+N");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);
 
    CreateStatusBar();
    SetStatusText("Welcome to DeskUp!");

    Bind(wxEVT_MENU, &DeskUpFrame::OnAdd, this, wxID_ADD);
    Bind(wxEVT_MENU, &DeskUpFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &DeskUpFrame::OnExit, this, wxID_EXIT);
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

        if(!workspaceName.empty()){
            break;
        }
        
        wxMessageBox("Your workspace name cannot be empty!", "Workspace name", wxOK | wxICON_INFORMATION);
    }
    
    std::cout << workspaceName;

    if (!DeskUpWindow::saveAllWindowsLocal(workspaceName.ToStdString())){
        wxMessageBox("Something went wrong! Please restart Desk Up", "Desk Up error", wxOK | wxICON_INFORMATION);
        wxExit();
    }

}