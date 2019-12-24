//
// Created by erik on 15-12-19.
//
#include "test.h"

class OverloadingExample {
private:
    Number myNumber;
    String myString;
public:
    OverloadingExample(Number n, String s) : myNumber(n), myString(s) {}

    String toString() {
        return myString + " " + std::to_string(myNumber);
    }

    String concat(OverloadingExample *other) {
        return myString + other->myString;
    }

    bool eq(OverloadingExample *other) {
        return myNumber == other->myNumber;
    }
};

TEST(OverloadingTest, testOverloading) {
    ClassPrototype *prototype = ClassPrototypeBuilder("OverloadingExample")
            .overload("__tostring", mk_method(&OverloadingExample::toString))
            .overload("__concat", mk_method(&OverloadingExample::concat))
            .overload("__eq", mk_method(&OverloadingExample::eq))
            .build();

    OverloadingExample e1(3, "Example 1"), e2(1, "Example 2");

    Lua _lua;
    auto retVal = _lua.add<OverloadingExample>(prototype)
        .set("e1", &e1)
        .set("e2", &e2)
        .file("o", "test/scripts/overloading.lua")
        .call("o");

    ASSERT_TRUE(retVal->is<Boolean>());
    ASSERT_TRUE(retVal->to<Boolean>());
}
