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

TEST_F(high_end_user_test, test_unm) {
    tlw::vec4 v2 = lua.src<tlw::vec4>("v1 = vec4(5) return -v1")();
    ASSERT_EQ(-5, v2.x);
    ASSERT_EQ(-5, v2.y);
    ASSERT_EQ(-5, v2.z);
    ASSERT_EQ(-5, v2.w);

    v2 = lua.src<tlw::vec4>("v1 = vec4.new(5) return -v1")();
    ASSERT_EQ(-5, v2.x);
    ASSERT_EQ(-5, v2.y);
    ASSERT_EQ(-5, v2.z);
    ASSERT_EQ(-5, v2.w);
}

TEST_F(high_end_user_test, test_len) {
    float l = lua.src<float>("v1 = vec4(5) return #v1")();
    ASSERT_EQ(tlw::vec4(5).length(), l);
    l = lua.src<float>("v1 = vec4.new(5) return #v1")();
    ASSERT_EQ(tlw::vec4(5).length(), l);
}
