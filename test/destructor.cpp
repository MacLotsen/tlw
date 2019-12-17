//
// Created by erik on 14-12-19.
//

//
// Created by erik on 14-12-19.
//
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
    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("Example")
            .destructor(mk_function(&DestructorExample::destroy))
            .build();

    // Do not use a stack variable, since it will be destroyed
    auto e = new DestructorExample;

    Lua _lua;
    _lua.file("d", "scripts/destructor.lua")
            .add<DestructorExample>(propertyPrototype)
            .set("example", e)
            .call("d");

    ASSERT_TRUE(destroyed);
}
