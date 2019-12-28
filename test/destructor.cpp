//
// Created by erik on 14-12-19.
//
#include <tlw/wrapping.hpp>
#include <tlw/types.hpp>
#include "destr_example.h"
#include "test.h"

// TODO: Can be merged generically

TEST(DestructorTest, testDestructor) {
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("Example")
            .destructor(mk_function(&DestructorExample::destroy))
            .build();

    // Do not use a stack variable, since it will be destroyed
    auto e = new DestructorExample;

    Lua _lua;
    _lua.add<DestructorExample>(propertyPrototype)
            .global("example", e)
            .src<LuaFunction<>>(DestructorExample::script)();

    ASSERT_TRUE(DestructorExample::destroyed);
}
