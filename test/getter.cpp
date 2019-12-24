//
// Created by erik on 14-12-19.
//
#include "test.h"
#include <tlw/types.hpp>
#include <tlw/wrapper.h>

class GetterExample {
private:
    std::string property;
public:
    explicit GetterExample(std::string p) : property(p) {}
    std::string getProperty() { return property; }
};


TEST(GetterTest, testGetter) {
    Lua _lua;
    PrettyClassPrototype *propertyPrototype = PrettyClassPrototypeBuilder("PropertyExample")
            .getter("property", mk_function(&GetterExample::getProperty))
            .build();

    GetterExample example {"Property"};
    auto r = _lua.add<GetterExample>(propertyPrototype)
            .global("example", &example)
            .file<LuaFunction<std::string()>>("test/scripts/getter.lua")();

    ASSERT_EQ("Property", r);
}
