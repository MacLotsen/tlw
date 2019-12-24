//
// Created by erik on 14-12-19.
//
#include "test.h"
#include <slua/wrapper.h>
#include <slua/types.hpp>

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

    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("Example")
            .constructor(mk_function(&ConstructorExample::create))
            .build();

    _lua.add<ConstructorExample>(propertyPrototype)
            .file<LuaFunction<>>("scripts/constructor.lua")();

    ASSERT_EQ(5, examples.size());
    for (auto v: examples) {
        delete v;
    }
    examples.clear();
}

TEST(ConstructorTest, testPersistence) {
    {
        Lua _lua;

        PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("Example")
                .constructor(mk_function(&ConstructorExample::create))
                .build();

        _lua.add<ConstructorExample>(propertyPrototype)
                .file<LuaFunction<>>("scripts/constructor.lua")();
    }
    ASSERT_EQ(5, examples.size());
    for (auto v: examples) {
        delete v;
    }
    examples.clear();
}
