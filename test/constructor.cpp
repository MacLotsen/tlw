//
// Created by erik on 14-12-19.
//
#include "test.h"

class ConstructorExample;

static std::vector<ConstructorExample *> examples;

class ConstructorExample {
public:
    ConstructorExample() {
        examples.push_back(this);
    }

    static ConstructorExample *create() {
        return new ConstructorExample;
    }
};

TEST(ConstructorTest, testConstructor) {
    Lua _lua;

    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("Example")
            .constr(mk_cfunc(&ConstructorExample::create))
            .build();

    _lua.file("c", "scripts/constructor.lua")
            .add<ConstructorExample>(propertyPrototype)
            .call("c");

    ASSERT_EQ(5, examples.size());
    for (auto v: examples) {
        delete v;
    }
    examples.clear();
}

TEST(ConstructorTest, testPersistence) {
    {
        Lua _lua;

        ClassPrototype *propertyPrototype = ClassPrototypeBuilder("Example")
                .constr(mk_cfunc(&ConstructorExample::create))
                .build();

        _lua.file("c", "scripts/constructor.lua")
                .add<ConstructorExample>(propertyPrototype)
                .call("c");
    }
    ASSERT_EQ(5, examples.size());
    for (auto v: examples) {
        delete v;
    }
    examples.clear();
}
