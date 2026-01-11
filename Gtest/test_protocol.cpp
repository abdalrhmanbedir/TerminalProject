#include <gtest/gtest.h>
#include "Protocol.h"

TEST(Protocol, ParseAuth)
{
    Command c = Protocol::parseLine("AUTH user,pass");
    EXPECT_EQ(c.verb.toStdString(), "AUTH");
    EXPECT_EQ(c.rest.toStdString(), "user,pass");
}
