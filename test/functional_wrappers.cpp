//
// Created by erik on 28-12-19.
//
#include <tlw/types.hpp>
#include "test.h"


TEST(FunctionalWrappers, testCFunctions) {
    auto f = lua.src<LuaFunction<double()>>(FunctionExample::script);
    ASSERT_EQ(5.0, f());
}

TEST(FunctionalWrappers, testScriptWithZeroReturnValues) {
    ASSERT_NO_THROW(lua.src<LuaFunction<>>(ScriptExample::noopScript)());
}

TEST(FunctionalWrappers, testScriptWithSingleReturnValue) {
    auto f = lua.src<LuaFunction<double()>>(ScriptExample::singleReturnScript);
    ASSERT_EQ(0, f());
}

TEST(FunctionalWrappers, testScriptWithMultiReturnValues) {
    auto f = lua.src<LuaFunction<std::tuple<bool, double, std::string>()>>(ScriptExample::multiReturnScript);

    bool b;
    double d;
    std::string s;
    std::tie(b, d, s) = f();

    ASSERT_TRUE(b);
    ASSERT_EQ(2.5, d);
    ASSERT_EQ("string", s);
}
