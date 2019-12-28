//
// Created by erik on 14-12-19.
//
#include <tlw/wrapping.hpp>
#include <tlw/types.hpp>
#include "test.h"
#include "setter_example.h"

// TODO: Can be merged generically

TEST(SetterTest, testSetter) {
    SetterExample example{"Property"};
    lua.global("example", &example)
            .src<LuaFunction<>>(SetterExample::script)();

    ASSERT_EQ("property changed", example.get());
}
