//
// Created by erik on 14-12-19.
//
#include "test.h"
#include <tlw/types.hpp>
#include "getter_example.h"

// TODO: Can be merged generically

TEST(GetterTest, testGetter) {
    GetterExample example {"Property"};
    auto r = lua.global("example", &example).src<LuaFunction<std::string()>>(GetterExample::script)();
    ASSERT_EQ("Property", r);
}
