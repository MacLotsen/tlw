//
// Created by erik on 14-12-19.
//
#include <tlw/wrapping.hpp>
#include <tlw/types.hpp>
#include "test.h"
#include "method_example.h"

// TODO: Can be merged generically


TEST(MethodTest, testMethods) {
    MethodExample e;

    auto retValue = lua.global("example", &e)
            .src<LuaFunction<double()>>(MethodExample::script)();

    ASSERT_EQ(15, e.getCallMask());
    ASSERT_EQ(10.0, retValue);
}
