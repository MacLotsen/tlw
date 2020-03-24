/*
 * Typed Lua Wrapping
 * Copyright (C) 2019  Erik Nijenhuis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <gtest/gtest.h>
#include <tlw/old/_types.hpp>
#include "tlw/old/_stack.hpp"


TEST(StackTest, testValueTypes) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test boolean
    lua_pushboolean(L, true);
    ASSERT_TRUE(s.pop<bool>());

    ASSERT_EQ(0, lua_gettop(L));

    // Test Number
    lua_pushnumber(L, 2.5);
    auto myNumber = s.pop<double>();
    ASSERT_EQ(2.5, myNumber);

    ASSERT_EQ(0, lua_gettop(L));

    // Test C String
    lua_pushstring(L, "LUA");
    lua_pushvalue(L, -1);
    auto cstr = s.pop<const char *>();
    ASSERT_STREQ("LUA", cstr);

    ASSERT_EQ(1, lua_gettop(L));

    auto str = s.pop<std::string>();
    ASSERT_STREQ("LUA", str.c_str());

    ASSERT_EQ(0, lua_gettop(L));
    lua_close(L);
}

TEST(StackTest, testVector) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test vector
    std::vector<double> values = {1, 4, 9};
    s.push(values);

    auto l = s.pop<std::vector<double>>();
    ASSERT_EQ(3, l.size());

    ASSERT_EQ(1, l.at(0));
    ASSERT_EQ(4, l.at(1));
    ASSERT_EQ(9, l.at(2));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

TEST(StackTest, testTuple) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test tuple
    s.push<std::tuple<bool, double, std::string>>({true, 2.5, "LUA"});
    ASSERT_EQ(1, lua_gettop(L));

    auto t = s.pop<std::tuple<bool, double, std::string>>();
    ASSERT_TRUE(std::get<0>(t));
    ASSERT_EQ(2.5, std::get<1>(t));
    ASSERT_EQ("LUA", std::get<2>(t));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

TEST(StackTest, testPopArgumentsAsTuple) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    s.push(true, 2.5, "LUA");
    ASSERT_EQ(3, lua_gettop(L));

    auto t = s.pop<bool, double, std::string>();
    ASSERT_TRUE(std::get<0>(t));
    ASSERT_EQ(2.5, std::get<1>(t));
    ASSERT_EQ("LUA", std::get<2>(t));

    ASSERT_EQ(0, lua_gettop(L));

    // Test with minimum types
    s.push(true, false);

    ASSERT_EQ(2, lua_gettop(L));

    auto t2 = s.pop<bool, bool>();
    ASSERT_TRUE(std::get<0>(t2));
    ASSERT_FALSE(std::get<1>(t2));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

TEST(StackTest, testTable) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test table/struct
    lua_newtable(L);
    lua_pushnumber(L, 1.5);
    lua_setfield(L, -2, "number");
    lua_pushstring(L, "a string");
    lua_setfield(L, -2, "str");

    {
        auto t = s.pop<LuaTable>();
        ASSERT_EQ(1.5, t.get<double>("number"));
        ASSERT_EQ("a string", t.get<std::string>("str"));
    }

    lua_close(L);
}

TEST(StackTest, testLuaFunctions) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    luaL_dostring(L, "r1 = nil\n"
                     "r3 = nil\n"
                     "\n"
                     "function f1()\n"
                     "    r1 = true\n"
                     "end\n"
                     "\n"
                     "function f2()\n"
                     "    return true\n"
                     "end\n"
                     "\n"
                     "function f3(s1, s2)\n"
                     "    r3 = s1 .. ' ' .. s2\n"
                     "end\n"
                     "\n"
                     "function f4(n1, n2)\n"
                     "    return n1 + n2\n"
                     "end");

    // Let the functions live within an active lua state (see last line)
    {
        lua_getglobal(L, "f1");
        auto f1 = s.pop<LuaFunction<>>();
        f1();
        lua_getglobal(L, "r1");
        ASSERT_TRUE(s.pop<bool>());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f2");
        auto f2 = s.pop<LuaFunction<bool()>>();
        ASSERT_TRUE(f2());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f3");
        auto f3 = s.pop<LuaFunction<void(std::string, std::string)>>();
        f3("John", "Doe");
        lua_getglobal(L, "r3");
        ASSERT_EQ("John Doe", s.pop<std::string>());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f4");
        auto f4 = s.pop<LuaFunction<double(double, double)>>();
        ASSERT_EQ(5.0, f4(2.5, 2.5));

        ASSERT_EQ(0, lua_gettop(L));
    }

    lua_close(L);
}

TEST(StackTest, testScriptAsFunction) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    luaL_loadstring(L, "return true, 1.0, 'true'\n");
    {
        auto f = s.pop<LuaFunction<std::tuple<bool, double, std::string>()>>();

        ASSERT_EQ(0, lua_gettop(L));

        bool a;
        double b;
        std::string c;
        std::tie(a, b, c) = f();
        ASSERT_TRUE(a);
        ASSERT_EQ(1, b);
        ASSERT_EQ("true", c);
    }
    lua_close(L);
}

TEST(StackTest, testScriptWithWrongReturnTypes) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    double num;
    luaL_loadstring(L, "return 'string'");
    {
        auto f = s.pop<LuaFunction<double()>>();

        ASSERT_EQ(0, lua_gettop(L));

        ASSERT_NO_THROW(num = f());
        ASSERT_EQ(0, num);
    }

    lua_close(L);
}
