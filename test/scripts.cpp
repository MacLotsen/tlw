//
// Created by enijenhuis on 11-12-2019.
//
#include "test.h"
#include <tlw/types.hpp>

class ScriptsTest : public ::testing::Test {};

TEST_F(ScriptsTest, testZeroReturnValues) {
    ASSERT_NO_THROW(lua.file<LuaFunction<>>("test/scripts/noop.lua")());
}

TEST_F(ScriptsTest, testSingleReturnValue) {
    auto f = lua.file<LuaFunction<double()>>("test/scripts/single_return.lua");
    ASSERT_EQ(0, f());
}

TEST_F(ScriptsTest, testMultiReturnValues) {
    auto f = lua.file<LuaFunction<std::tuple<bool, double, std::string>()>>("test/scripts/return_primitives.lua");

    bool b;
    double d;
    std::string s;
    std::tie(b, d, s) = f();

    ASSERT_TRUE(b);
    ASSERT_EQ(2.5, d);
    ASSERT_EQ("string", s);
}
