#include <gtest/gtest.h>

#include "desk_up.h"

TEST(DeskUpTestLoading, ClassInitializes){
    const DeskUpApp * app = new DeskUpApp();
    delete app;
}