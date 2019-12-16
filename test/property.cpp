//
// Created by erik on 14-12-19.
//
#include "test.h"

struct PropertyExample {
    String property;
};


TEST(PropertyTest, testProperty) {
    Lua _lua;
    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("PropertyExample")
            .property("property", mk_property(&PropertyExample::property))
            .build();

    PropertyExample example {"Property"};
    auto r = _lua.add<PropertyExample>(propertyPrototype)
            .set("example", &example)
            .file("p", "scripts/property.lua")
            .call("p");

    ASSERT_TRUE(r->is<String>());
    ASSERT_EQ("Property", r->to<String>());
    ASSERT_EQ("property changed", example.property);
}
