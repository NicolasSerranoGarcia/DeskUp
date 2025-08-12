#include "desk_up_frame.h"

enum
{
    ID_Hello = 1
};


DeskUpFrame::DeskUpFrame()
        : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);
 
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_MENU, &DeskUpFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &DeskUpFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &DeskUpFrame::OnExit, this, wxID_EXIT);
}

void DeskUpFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void DeskUpFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void DeskUpFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}