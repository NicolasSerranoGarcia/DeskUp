#ifndef DESKUPERRORGUICONVERTER_H
#define DESKUPERRORGUICONVERTER_H

#include <wx/msgdlg.h>
#include "desk_up_error.h"

namespace DeskUp::UI {

class ErrorAdapter {
public:
    static int showError(const DeskUp::Error& err);
    static wxString getUserMessage(const DeskUp::Error& err);

private:
    static std::pair<wxString, long> mapLevel(Level lvl);
};

} 

#endif