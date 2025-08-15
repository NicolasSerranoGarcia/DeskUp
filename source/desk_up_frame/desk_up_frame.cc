#include "desk_up_frame.h"

DeskUpFrame::DeskUpFrame()
        : wxFrame(nullptr, wxID_ANY, "DeskUp")
{
    #ifdef WIN32
        SetIcon(wxICON(IDI_APPLICATION));
    #endif
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
    wxLogMessage("You will add a workspace here :)! Be patient");
}