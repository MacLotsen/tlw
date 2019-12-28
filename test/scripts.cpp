//
// Created by enijenhuis on 11-12-2019.
//
#include <tlw/types.hpp>
#include "test.h"
#include "script_example.h"

// TODO: Can be merged generically

TEST(ScriptsTest, testZeroReturnValues) {
    ASSERT_NO_THROW(lua.src<LuaFunction<>>(ScriptExample::noopScript)());
}

TEST(ScriptsTest, testSingleReturnValue) {
    auto f = lua.src<LuaFunction<double()>>(ScriptExample::singleReturnScript);
    ASSERT_EQ(0, f());
}

TEST(ScriptsTest, testMultiReturnValues) {
    auto f = lua.src<LuaFunction<std::tuple<bool, double, std::string>()>>(ScriptExample::multiReturnScript);

    bool b;
    double d;
    std::string s;
    std::tie(b, d, s) = f();

    ASSERT_TRUE(b);
    ASSERT_EQ(2.5, d);
    ASSERT_EQ("string", s);
}
