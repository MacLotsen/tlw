//
// Created by enijenhuis on 16-12-2019.
//
#include <slua/types.hpp>
#include <slua/wrapper.h>
#include <cmath>
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

    double addition(double other) {
        return myNumber + other;
    }

    double subtraction(double other) {
        return myNumber - other;
    }

    double multiplication(double other) {
        return myNumber * other;
    }

    double division(double other) {
        return myNumber / other;
    }

    int modulo(int other) {
        return int(myNumber) % other;
    }

    double power(double other) {
        return std::pow(myNumber, other);
    }

    bool equality(NumberExample* other) {
        return myNumber == other->myNumber;
    }

    bool lessThan(NumberExample* other) {
        return myNumber < other->myNumber;
    }

    bool lessThanOrEqual(NumberExample* other) {
        return myNumber <= other->myNumber;
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

    PrettyClassPrototype *prototype = PrettyClassPrototypeBuilder("NumberExample")
            .inverse(mk_function(&NumberExample::inverse))
            .addition(mk_function(&NumberExample::addition))
            .subtraction(mk_function(&NumberExample::subtraction))
            .multiplication(mk_function(&NumberExample::multiplication))
            .division(mk_function(&NumberExample::division))
            .modulo(mk_function(&NumberExample::modulo))
            .power(mk_function(&NumberExample::power))
            .equality(mk_function(&NumberExample::equality))
            .lessThan(mk_function(&NumberExample::lessThan))
            .lessThanOrEqual(mk_function(&NumberExample::lessThanOrEqual))
            .build();


    NumberExample ne{}, ne2{2.0};
    auto r = _lua.add<NumberExample>(prototype)
            .global("ne", &ne)
            .global("ne2", &ne2)
            .file<LuaFunction<std::tuple<double, double, double, double, double, double, double, bool, bool, bool>()>>("test/scripts/number_overloading.lua")();

    ASSERT_EQ(-1, std::get<0>(r));
    ASSERT_EQ(2, std::get<1>(r));
    ASSERT_EQ(0, std::get<2>(r));
    ASSERT_EQ(2, std::get<3>(r));
    ASSERT_EQ(1, std::get<4>(r));
    ASSERT_EQ(1, std::get<5>(r));
    ASSERT_EQ(1, std::get<6>(r));
    ASSERT_TRUE(std::get<7>(r));
    ASSERT_TRUE(std::get<8>(r));
    ASSERT_TRUE(std::get<9>(r));
}

TEST(OverloadingTest, testString) {

}

TEST(OverloadingTest, testBinary) {

}
