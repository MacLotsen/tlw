//
// Created by enijenhuis on 11-12-2019.
//
#include "test.h"

class ScriptsTest : public ::testing::Test {};

TEST_F(ScriptsTest, testZeroReturnValues) {
    AbstractValue *returnVal = lua.call("noop");
    ASSERT_EQ(nullptr, returnVal);
}

TEST_F(ScriptsTest, testSingleReturnValue) {
    AbstractValue *returnVal = lua.call("single");;
    ASSERT_NE(nullptr, returnVal);
    ASSERT_TRUE(returnVal->is<Number>());
    ASSERT_EQ(0, returnVal->to<Number>());
}

TEST_F(ScriptsTest, testMultiReturnValues) {
    AbstractValue *returnVal = lua.call("primitives");
    ASSERT_NE(nullptr, returnVal);
    ASSERT_TRUE(returnVal->is<List>());
    ASSERT_LT(1, returnVal->to<List>().size());
}
