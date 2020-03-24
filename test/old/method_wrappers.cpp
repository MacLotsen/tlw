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

#include <tlw/old/_types.hpp>
#include "test.h"


TEST(MethodWrappers, testConstructor) {
    lua.src(ConstructorExample::script)();

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
                .src(ConstructorExample::script)();
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
            .setObject("example", e)
            .src(DestructorExample::script)();

    ASSERT_TRUE(DestructorExample::destroyed);
}

TEST(MethodWrappers, testGetter) {
    GetterExample example{"Property"};
    auto r = lua.setObject("example", &example).src<LuaFunction<std::string()>>(GetterExample::script)();
    ASSERT_EQ("Property", r);
}

TEST(MethodWrappers, testSetter) {
    SetterExample example{"Property"};
    lua.setObject("example", &example)
            .src(SetterExample::script)();

    ASSERT_EQ("property changed", example.get());
}

TEST(MethodWrappers, testProperty) {
    Lua _lua;
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("PropertyExample")
            .property("property", mk_property(&PropertyExample::property))
            .build();

    PropertyExample example{"Property"};
    auto r = _lua.add<PropertyExample>(propertyPrototype)
            .setObject("example", &example)
            .src<LuaFunction<std::string()>>(PropertyExample::script)();

    ASSERT_EQ("Property", r);
    ASSERT_EQ("property changed", example.property);
}

TEST(MethodWrappers, testMethods) {
    MethodExample e;

    auto retValue = lua.setObject("example", &e)
            .src<LuaFunction<double()>>(MethodExample::script)();

    ASSERT_EQ(15, e.getCallMask());
    ASSERT_EQ(10.0, retValue);
}

TEST(MethodWrappers, testNumber) {
    NumberExample ne{}, ne2{2.0};
    const NumberExample cne;
    auto r = lua
            .setObject("ne", &ne)
            .setObject("ne2", &ne2)
            .setObject("cne", &cne)
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
            .setObject("example", &e)
            .src<LuaFunction<const char *()>>(StringExample::script)();

    ASSERT_STREQ(r, "C++ in Lua");
}

TEST(MethodWrappers, testConstRefObject) {
    MethodExample me;

    auto f = lua.setObject("example", me)
            .src<LuaFunction<unsigned int()>>("result = example.self; example.method1(); return example.self.callMask");

    auto r = f();

    ASSERT_EQ(1, r);

    auto r2 = lua.get<const MethodExample &>("result");

    // Not the same pointer, yet still the same behaviour
    ASSERT_EQ(me.getCallMask(), r2.getCallMask());
}

TEST(MethodWrappers, testInheritance) {
    DerivativeExample de;
    de.number = 2.5;

    auto f = lua.setObject("example", de)
            .src(DerivativeExample::script);

    f();

    ASSERT_EQ(2.5, lua.get<double>("n"));
    ASSERT_EQ(5.0, de.number) << "Expect setters to be present in child class";
}
