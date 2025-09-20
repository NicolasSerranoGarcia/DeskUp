#ifndef DESKUPFRAME_H
#define DESKUPFRAME_H

#include <wx/wx.h>

class DeskUpFrame : public wxFrame{
    public:
        DeskUpFrame();

    private:
    void OnAdd(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

#endif