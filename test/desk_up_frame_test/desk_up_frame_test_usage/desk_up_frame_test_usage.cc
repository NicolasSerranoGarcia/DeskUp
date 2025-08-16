#include <gtest/gtest.h>

#include "desk_up_frame.h"

TEST(DeskUpFrameTestUsage, ClassInitializes){
    const DeskUpFrame * frame = new DeskUpFrame();
    delete frame;
}