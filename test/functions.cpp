//
// Created by enijenhuis on 12-12-2019.
//
#include "test.h"
#include <tlw/types.hpp>
#include <tlw/wrapper.h>

class FunctionsTest : public ::testing::Test {
};

double plus(double a, double b) {
    return a + b;
}

void noop() {

}

using F1 = double (*)(double, double);

TEST_F(FunctionsTest, testZeroReturnValues) {
    lua_CFunction p = mk_function(plus);
    lua_CFunction n = mk_function(noop);
    lua.global("plus", p);
    lua.global("noop", n);
    auto f = lua.file<LuaFunction<double()>>("test/scripts/plus.lua");
    ASSERT_EQ(5.0, f());
}
