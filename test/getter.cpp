//
// Created by erik on 14-12-19.
//
#include "test.h"

class GetterExample {
private:
    String property;
public:
    explicit GetterExample(String p) : property(p) {}
    String getProperty() { return property; }
};


TEST(GetterTest, testGetter) {
    Lua _lua;
    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("PropertyExample")
            .getter("property", mk_function(&GetterExample::getProperty))
            .build();

    GetterExample example {"Property"};
    auto r = _lua.add<GetterExample>(propertyPrototype)
            .set("example", &example)
            .file("p", "scripts/getter.lua")
            .call("p");

    ASSERT_TRUE(r->is<String>());
    ASSERT_EQ("Property", r->to<String>());
}
