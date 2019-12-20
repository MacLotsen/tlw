//
// Created by enijenhuis on 16-12-2019.
//
#include "test.h"

class NumberExample {
private:
    double myNumber;
public:
    NumberExample() : myNumber(1.0) {}
    explicit NumberExample(double n) : myNumber(n) {}
    double inverse(/*NumberExample *self*/) {
        // TODO: inv gives userdata twice??
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
//    Lua _lua;
//
//    ClassPrototype *prototype = ClassPrototypeBuilder("NumberExample")
//            .inverse(mk_function(&NumberExample::inverse))
//            .build();
//
//
//    NumberExample ne{};
//    _lua.add<NumberExample>(prototype)
//            .set("ne", &ne)
//            .file("cn", "scripts/custom_number.lua")
//            .call("cn");
}

TEST(OverloadingTest, testString) {

}

TEST(OverloadingTest, testBinary) {

}
