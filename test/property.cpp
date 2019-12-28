//
// Created by erik on 14-12-19.
//
#include <tlw/wrapping.hpp>
#include <tlw/types.hpp>
#include "test.h"
#include "prop_example.h"

// TODO: Can be merged generically


TEST(PropertyTest, testProperty) {
    Lua _lua;
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("PropertyExample")
            .property("property", mk_property(&PropertyExample::property))
            .build();

    PropertyExample example {"Property"};
    auto r = _lua.add<PropertyExample>(propertyPrototype)
            .global("example", &example)
            .src<LuaFunction<std::string()>>(PropertyExample::script)();

    ASSERT_EQ("Property", r);
    ASSERT_EQ("property changed", example.property);
}
