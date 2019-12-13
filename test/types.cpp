//
// Created by enijenhuis on 11-12-2019.
//
#include "test.h"

class TypesTest : public ::testing::Test { };

TEST_F(TypesTest, testBoolean) {
    List returns = lua.call("primitives")->to<List>();
    AbstractValue *val = returns.at(0);
    ASSERT_TRUE(val->is<Boolean>());
    ASSERT_TRUE(val->to<Boolean>());
}

TEST_F(TypesTest, testNumber) {
    List returns = lua.call("primitives")->to<List>();
    AbstractValue *val = returns.at(1);
    ASSERT_TRUE(val->is<Number>());
    ASSERT_EQ(2.5, val->to<Number>());
}

TEST_F(TypesTest, testString) {
    List returns = lua.call("primitives")->to<List>();
    AbstractValue *val = returns.at(2);
    ASSERT_TRUE(val->is<String>());
    ASSERT_EQ("string",  val->to<String>());
}

TEST_F(TypesTest, testList) {
    AbstractValue *val = lua.call("array");
    ASSERT_TRUE(val->is<List>());
    List l = val->to<List>();
    ASSERT_EQ(1, l.size());
    ASSERT_EQ("list", l.at(0)->to<String>());
}

TEST_F(TypesTest, testTable) {
    AbstractValue *val = lua.call("table");
    ASSERT_TRUE(val->is<Table>());
    Table t = val->to<Table>();
    ASSERT_EQ(1, t.count("key"));
    ASSERT_EQ("value", t["key"]->to<String>());
}
