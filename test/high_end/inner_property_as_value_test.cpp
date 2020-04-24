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



TEST_F(high_end_user_test, test_inner_property_as_value_test) {
    luaL_openlibs(L);
    lua.src("e1 = entity.new() e1.position.x = 5")();

    tlw::entity *e1 = lua["e1"];

    ASSERT_EQ(5, e1->position.x);

    lua.src("e1.position.x = 6 print(e1.position.x)")();

    ASSERT_EQ(6, e1->position.x);
}

TEST_F(high_end_user_test, test_inner_property_and_ud_as_value_test) {
    luaL_openlibs(L);
    lua.src("e1 = entity() e1.position.x = 5")();

    tlw::entity e1 = lua["e1"];

    ASSERT_EQ(5, e1.position.x);

    lua.src("e1.position.x = 6 print(e1.position.x)")();

    e1 = lua["e1"];

    ASSERT_EQ(6, e1.position.x);
}
