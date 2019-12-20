//
// Created by enijenhuis on 16-12-2019.
//
#include <slua/types.hpp>
#include <slua/wrapper.h>
#include "test.h"

class NumberExample {
private:
    double myNumber;
public:
    NumberExample() : myNumber(1.0) {}
    explicit NumberExample(double n) : myNumber(n) {}

    /**
     * For complicated reasons the unary minus provides the userdata twice...
     * See (http://lua-users.org/lists/lua-l/2016-10/msg00351.html)
     *
     * @param self
     * @return
     */
    double inverse(NumberExample *) {
        return -myNumber;
    }
};

class StringExample {
private:
    std::string myString;
public:
};

class BinaryExample {
private:
    uint8_t myByte;
public:

};

TEST(OverloadingTest, testNumber) {
    Lua _lua;

    ClassPrototype *prototype = ClassPrototypeBuilder("NumberExample")
            .inverse(mk_function(&NumberExample::inverse))
            .build();


    NumberExample ne{};
    auto r = _lua.add<NumberExample>(prototype)
            .set("ne", &ne)
            .file<LuaFunction<double()>>("scripts/custom_number.lua")();

    ASSERT_EQ(-1, r);
}

TEST(OverloadingTest, testString) {

}

TEST(OverloadingTest, testBinary) {

}
