//
// Created by enijenhuis on 12-12-2019.
//
#include <tlw/types.hpp>
#include <tlw/wrapping.hpp>
#include "test.h"
#include "func_example.h"

// TODO: Can be merged generically

TEST(FunctionsTest, testZeroReturnValues) {
    auto f = lua.src<LuaFunction<double()>>(FunctionExample::script);
    ASSERT_EQ(5.0, f());
}
