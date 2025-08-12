#include "desk_up.h"
#include "desk_up_frame/desk_up_frame.h"

bool DeskUpApp::OnInit(){
    DeskUpFrame * frame = new DeskUpFrame();
    frame->Show();
    return true;
}