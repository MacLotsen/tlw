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
    double inverse() {
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
    _lua.add<NumberExample>(prototype)
            .set("ne", &ne)
            .file<LuaFunction<>>("scripts/custom_number.lua")();
}

TEST(OverloadingTest, testString) {

}

TEST(OverloadingTest, testBinary) {

}
