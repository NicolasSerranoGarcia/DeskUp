#include <wx/wx.h>


class DeskUpFrame : public wxFrame{
    public:
        DeskUpFrame();

    private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};