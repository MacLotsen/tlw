//
// Created by erik on 14-12-19.
//
#include "test.h"

class SetterExample {
private:
    String property;
public:
    explicit SetterExample(String p) : property(p) {}
    void setProperty(String p) { property = p; }
    String get() {
        return property;
    }
};


TEST(SetterTest, testSetter) {
    Lua _lua;
    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("PropertyExample")
            .setter("property", mk_setter(&SetterExample::setProperty))
            .build();

    SetterExample example {"Property"};
    _lua.add<SetterExample>(propertyPrototype)
            .set("example", &example)
            .file("p", "scripts/setter.lua")
            .call("p");

    ASSERT_EQ("property changed", example.get());
}
