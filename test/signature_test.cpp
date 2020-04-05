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

    s.push(std::move(v1));
    ASSERT_TRUE(tlw::type_inspector<tlw::user_data_t<tlw::vec4>>::inspect(L));
    lua_getmetatable(L, -1);
    printf("type %s", luaL_typename(L, -1));
    lua_pop(L, 1);
//    lua_getfield(L, -1, "__name");
//    printf("%s\n", s.pop<const char *>());
    lua_setglobal(L, "v1");
    s.push(m1);
    lua_setglobal(L, "m1");

    if (luaL_dostring(L, "print(v1.z) e1 = entity(v1) e2 = entity(m1)")) {
        FAIL() << "Couldn't execute script '" << lua_tostring(L, -1) << "'";
    }

    lua_getglobal(L, "e1");
    auto e1 = s.pop<tlw::entity*>();
    lua_getglobal(L, "e2");
    auto e2 = s.pop<tlw::entity*>();

    ASSERT_EQ(1, e1->position.z);
    ASSERT_EQ(1, e2->model.z[0]);
}
