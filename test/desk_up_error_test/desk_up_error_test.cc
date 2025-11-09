#include <gtest/gtest.h>

#include <vector>

#include "desk_up_error.h"

using namespace DeskUp;

std::vector<Level> l = {Level::Debug, Level::Default, Level::Error, Level::Fatal, Level::Info, Level::None, Level::Retry, Level::Warning};

TEST(DeskUpErrorTest, checkLevel){
    for(unsigned int i = 0; i < l.size(); i++){
        Error err(l[i], ErrType::None, 0, "");
        ASSERT_EQ(err.level(), l[i]);
    }
}

