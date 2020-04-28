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

#include "high_end_user_test.h"


TEST_F(high_end_user_test, test_any_value) {
    float n1 = 5.5;
    const char *s1 = "string";

    lua.set("n1", n1);
    lua.set("s1", s1);

    const char *s2 = lua["s1"];
    ASSERT_STREQ(s1, s2);
    float n2 = lua["n1"];
    ASSERT_EQ(n1, n2);

    tlw::define<tlw::example>("example_value")
            .build();

    tlw::meta_table_registry<tlw::example>::expose(L);
    tlw::example val1(5.5);
    lua.set("ud", std::move(val1));

    tlw::example val2 = lua["ud"];
    if (val2)
        val2.print();

    luaL_dostring(L, "t1 = { { 'Hallo' } }");

    tlw::table t = lua["t1"][1];
    const char *str2 = t[1];

    ASSERT_STREQ("Hallo", str2);
}
