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

TEST_F(high_end_user_test, test_overloaded_method) {
    float n = 5;
    auto v1 = tlw::vec4(5.5);
    const tlw::vec4 v2 = tlw::vec4(4.4);

    lua.set("n", n);
    lua.set("v1", v1);
    lua.set<const tlw::vec4&>("v2", v2);

    ASSERT_NO_THROW(lua.src("v1.add(v2)")());
    ASSERT_NO_THROW(lua.src("v1.add(n)")());
}
