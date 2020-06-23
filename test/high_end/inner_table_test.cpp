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

TEST_F(high_end_user_test, test_inner_table) {
    lua.src("t1 = { t2 = { t3 = { a = 1.0 } } }")();

    float a = lua["t1"]["t2"]["t3"]["a"];
//    tlw::table t1 = lua["t1"];
//    tlw::table t2 = t1["t2"];
//    float a = t2["a"];

    ASSERT_EQ(1.0f, a);
}

TEST_F(high_end_user_test, test_inner_table_with_while) {
    std::vector<std::string> keys = {"t2", "t3"};
    lua.src("t1 = { t2 = { t3 = { a = 1.0 } } }")();

    tlw::table extra_ref = lua["t1"];
    tlw::table t = lua["t1"];

    for (std::string key : keys) {
        t = t[key];
    }
    float a = t["a"];
//    tlw::table t1 = lua["t1"];
//    tlw::table t2 = t1["t2"];
//    float a = t2["a"];

    ASSERT_EQ(1.0f, a);
}
