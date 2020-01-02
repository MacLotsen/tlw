/*
 * Typed Lua Wrapping
 * Copyright (C) 2019  Erik Nijenhuis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <tlw/types.hpp>
#include "test.h"


TEST(MethodWrappers, testConstructor) {
    lua.src<LuaFunction<>>(ConstructorExample::script)();

    ASSERT_EQ(5, ConstructorExample::examples.size());
    for (auto v: ConstructorExample::examples) {
        delete v;
    }
    ConstructorExample::examples.clear();
}

TEST(MethodWrappers, testPersistence) {
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

TEST(MethodWrappers, testDestructor) {
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

TEST(MethodWrappers, testGetter) {
    GetterExample example{"Property"};
    auto r = lua.global("example", &example).src<LuaFunction<std::string()>>(GetterExample::script)();
    ASSERT_EQ("Property", r);
}

TEST(MethodWrappers, testSetter) {
    SetterExample example{"Property"};
    lua.global("example", &example)
            .src<LuaFunction<>>(SetterExample::script)();

    ASSERT_EQ("property changed", example.get());
}

TEST(MethodWrappers, testProperty) {
    Lua _lua;
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("PropertyExample")
            .property("property", mk_property(&PropertyExample::property))
            .build();

    PropertyExample example{"Property"};
    auto r = _lua.add<PropertyExample>(propertyPrototype)
            .global("example", &example)
            .src<LuaFunction<std::string()>>(PropertyExample::script)();

    ASSERT_EQ("Property", r);
    ASSERT_EQ("property changed", example.property);
}

TEST(MethodWrappers, testMethods) {
    MethodExample e;

    auto retValue = lua.global("example", &e)
            .src<LuaFunction<double()>>(MethodExample::script)();

    ASSERT_EQ(15, e.getCallMask());
    ASSERT_EQ(10.0, retValue);
}

TEST(MethodWrappers, testNumber) {
    NumberExample ne{}, ne2{2.0};
    auto r = lua
            .global("ne", &ne)
            .global("ne2", &ne2)
            .src<LuaFunction<std::tuple<double, double, double, double, double, double, double, bool, bool, bool>()>>(
                    NumberExample::script)();

    ASSERT_EQ(-1, std::get<0>(r));
    ASSERT_EQ(2, std::get<1>(r));
    ASSERT_EQ(0, std::get<2>(r));
    ASSERT_EQ(2, std::get<3>(r));
    ASSERT_EQ(1, std::get<4>(r));
    ASSERT_EQ(1, std::get<5>(r));
    ASSERT_EQ(1, std::get<6>(r));
    ASSERT_TRUE(std::get<7>(r));
    ASSERT_TRUE(std::get<8>(r));
    ASSERT_TRUE(std::get<9>(r));
}

TEST(MethodWrappers, testString) {
    StringExample e("C++");
    auto r = lua
            .global("example", &e)
            .src<LuaFunction<const char *()>>(StringExample::script)();

    ASSERT_STREQ(r, "C++ in Lua");
}
