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

#include <tlw/types.hpp>
#include "test.h"


TEST(FunctionalWrappers, testCFunctions) {
    auto f = lua.src<LuaFunction<double()>>(FunctionExample::script);
    ASSERT_EQ(5.0, f());
}

TEST(FunctionalWrappers, testScriptWithZeroReturnValues) {
    ASSERT_NO_THROW(lua.src<LuaFunction<>>(ScriptExample::noopScript)());
}

TEST(FunctionalWrappers, testScriptWithSingleReturnValue) {
    auto f = lua.src<LuaFunction<double()>>(ScriptExample::singleReturnScript);
    ASSERT_EQ(0, f());
}

TEST(FunctionalWrappers, testScriptWithMultiReturnValues) {
    auto f = lua.src<LuaFunction<std::tuple<bool, double, std::string>()>>(ScriptExample::multiReturnScript);

    bool b;
    double d;
    std::string s;
    std::tie(b, d, s) = f();

    ASSERT_TRUE(b);
    ASSERT_EQ(2.5, d);
    ASSERT_EQ("string", s);
}
