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

#include "user_test.h"

uint64_t inverse(uint64_t n) {
    return ~n;
}

TEST_F(user_test, test_cfunction1) {
    s.push(&inverse);

    ASSERT_TRUE(tlw::type_inspector<tlw::function_t>::inspect(L));

    unsigned long n = 5;
    s.push(n);
    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << lua_tostring(L, -1);
    }

    ASSERT_EQ(~n, s.pop<unsigned long>());

    lua_settop(L, 0);
}

void nothing() {

}

TEST_F(user_test, test_cfunction2) {
    s.push(&nothing);

    ASSERT_TRUE(tlw::type_inspector<tlw::function_t>::inspect(L));

    if (lua_pcall(L, 0, 0, 0)) {
        FAIL() << lua_tostring(L, -1);
    }

    lua_settop(L, 0);
}

const char * hello_world() {
    return "Hello World";
}

TEST_F(user_test, test_cfunction3) {
    s.push(&hello_world);

    ASSERT_TRUE(tlw::type_inspector<tlw::function_t>::inspect(L));

    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << lua_tostring(L, -1);
    }

    ASSERT_STREQ("Hello World", s.pop<const char *>());

    lua_settop(L, 0);
}

void example_load(const char * str) {
    // do stuff
    printf("Loaded %s", str);
}

TEST_F(user_test, test_cfunction4) {
    s.push(&example_load);

    ASSERT_TRUE(tlw::type_inspector<tlw::function_t>::inspect(L));

    s.push("rocket launcher");
    if (lua_pcall(L, 1, 0, 0)) {
        FAIL() << lua_tostring(L, -1);
    }

    lua_settop(L, 0);
}
