//
// Created by erik on 14-12-19.
//
#include "test.h"
#include <slua/wrapper.h>
#include <slua/types.hpp>

struct PropertyExample {
    std::string property;
};


TEST(PropertyTest, testProperty) {
    Lua _lua;
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("PropertyExample")
            .property("property", mk_property(&PropertyExample::property))
            .build();

    PropertyExample example {"Property"};
    auto r = _lua.add<PropertyExample>(propertyPrototype)
            .global("example", &example)
            .file<LuaFunction<std::string()>>("test/scripts/property.lua")();

    ASSERT_EQ("Property", r);
    ASSERT_EQ("property changed", example.property);
}
