//
// Created by erik on 14-12-19.
//

//
// Created by erik on 14-12-19.
//
#include <slua/wrapper.h>
#include <slua/types.hpp>
#include "test.h"

static bool destroyed = false;

class DestructorExample {
public:
    DestructorExample() = default;

    static void destroy(DestructorExample *instance) {
        delete instance;
        destroyed = true;
    }
};

TEST(DestructorTest, testDestructor) {
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("Example")
            .destructor(mk_function(&DestructorExample::destroy))
            .build();

    // Do not use a stack variable, since it will be destroyed
    auto e = new DestructorExample;

    Lua _lua;
    _lua.add<DestructorExample>(propertyPrototype)
            .global("example", e)
            .file<LuaFunction<>>("scripts/destructor.lua")();

    ASSERT_TRUE(destroyed);
}
