//
// Created by erik on 14-12-19.
//
#include "test.h"
#include "constr_example.h"
#include <tlw/wrapping.hpp>
#include <tlw/types.hpp>

// TODO: Can be merged generically

TEST(ConstructorTest, testConstructor) {
    lua.src<LuaFunction<>>(ConstructorExample::script)();

    ASSERT_EQ(5, ConstructorExample::examples.size());
    for (auto v: ConstructorExample::examples) {
        delete v;
    }
    ConstructorExample::examples.clear();
}

TEST(ConstructorTest, testPersistence) {
    auto constrPrototype = PrettyClassPrototypeBuilder("ConstructorExample")
            .constructor(mk_function(&ConstructorExample::create))
            .build();
    {
        Lua _lua;

        _lua.add<ConstructorExample>(constrPrototype)
                .src<LuaFunction<>>(ConstructorExample::script)();
    }
    ASSERT_EQ(5, ConstructorExample::examples.size());
    for (auto v: ConstructorExample::examples) {
        delete v;
    }
    ConstructorExample::examples.clear();
}
