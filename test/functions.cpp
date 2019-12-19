//
// Created by enijenhuis on 12-12-2019.
//
#include "test.h"

class FunctionsTest : public ::testing::Test {
};

double plus(double a, double b) {
    return a + b;
}

void noop() {

}

using F1 = double (*)(double, double);

TEST_F(FunctionsTest, testZeroReturnValues) {
    mkfunc(lua.getState(), plus);
    lua_setglobal(lua.getState(), "plus");
    lua_CFunction n = mk_function(noop);
//    lua.add("plus", p);
    lua.add("noop", n);
//    lua.call("plus");
    lua.file("plus", "scripts/plus.lua");
    ASSERT_EQ(5.0, lua.call("plus")->to<Number>());
}
