#include "../include/ftest.hpp"
#include "../include/add.hpp"

TEST(Add, this_suceeds)
{
    EXPECT_TRUE(add(2, 3) == 5);
    EXPECT_TRUE(true);
}

TEST(Add, this_fails)
{
    EXPECT_TRUE(false);
}
