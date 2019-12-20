//
// Created by erik on 14-12-19.
//
#include <slua/wrapper.h>
#include <slua/types.hpp>
#include <utility>
#include "test.h"

class SetterExample {
private:
    std::string property;
public:
    explicit SetterExample(std::string p) : property(std::move(p)) {}
    void setProperty(const std::string& p) { property = p; }
    std::string get() {
        return property;
    }
};


TEST(SetterTest, testSetter) {
    Lua _lua;
    ClassPrototype *propertyPrototype = ClassPrototypeBuilder("PropertyExample")
            .setter("property", mk_function(&SetterExample::setProperty))
            .build();

    SetterExample example {"Property"};
    _lua.add<SetterExample>(propertyPrototype)
            .set("example", &example)
            .file<LuaFunction<>>("scripts/setter.lua")();

    ASSERT_EQ("property changed", example.get());
}
