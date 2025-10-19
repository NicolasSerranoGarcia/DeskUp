#ifndef DESKUPFRAME_H
#define DESKUPFRAME_H

#include <wx/wx.h>
#include "desk_up_error.h"

class DeskUpFrame : public wxFrame{
    public:
        DeskUpFrame();

    private:
    void OnAdd(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnRestore(wxCommandEvent& event);

    static int showSaveSuccessful();

    static int showRestoreSuccessful();

    static int showMessageFromError(DeskUp::Error err);
};

#endif