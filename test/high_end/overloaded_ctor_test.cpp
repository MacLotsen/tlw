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



TEST_F(high_end_user_test, test_variable_constructor) {
    luaL_openlibs(L);
    lua_settop(L, 0);
    tlw::vec4 v1(1.0f);
    tlw::mat4 m1(1.0f);

    lua.set("v1", v1);
    lua.set("m1", m1);

    lua.src("print(v1.z) e1 = entity.new(v1) e2 = entity.new(m1)")();

    tlw::entity* e1 = lua["e1"];
    tlw::entity* e2 = lua["e2"];

    ASSERT_EQ(1, e1->position.z);
    ASSERT_EQ(1, e2->model.z[2]);
}
