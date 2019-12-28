//
// Created by enijenhuis on 16-12-2019.
//
#include <tlw/types.hpp>
#include <tlw/wrapping.hpp>
#include <cmath>
#include "test.h"
#include "number_example.h"

// TODO: Can be merged generically

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
    NumberExample ne{}, ne2{2.0};
    auto r = lua
            .global("ne", &ne)
            .global("ne2", &ne2)
            .src<LuaFunction<std::tuple<double, double, double, double, double, double, double, bool, bool, bool>()>>(NumberExample::script)();

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
