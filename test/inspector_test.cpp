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

#include "detail_test.h"

TEST_F(detail_test, test_inspector) {
    lua_pushboolean(L, magic_bool);
    inspect<tlw::bool_t>(true);
    inspect<tlw::number_t, tlw::string_t, tlw::table_t, tlw::cfunction_t, tlw::lua_example_t>(false);

    lua_pushnumber(L, magic_number);
    inspect<tlw::number_t>(true);
    inspect<tlw::bool_t, tlw::string_t, tlw::table_t, tlw::cfunction_t, tlw::lua_example_t>(false);

    lua_pushstring(L, magic_string);
    inspect<tlw::string_t>(true);
    inspect<tlw::bool_t, tlw::number_t, tlw::table_t, tlw::cfunction_t, tlw::lua_example_t>(false);

    lua_createtable(L, 0, 0);
    inspect<tlw::table_t>(true);
    inspect<tlw::bool_t, tlw::number_t, tlw::string_t, tlw::cfunction_t, tlw::lua_example_t>(false);

    lua_pushcfunction(L, &noop);
    inspect<tlw::cfunction_t>(true);
    inspect<tlw::bool_t, tlw::number_t, tlw::string_t, tlw::table_t, tlw::lua_example_t>(false);

    lua_newuserdata(L, sizeof(const tlw::example **));
    inspect<tlw::lua_example_t>(true);
    inspect<tlw::bool_t, tlw::number_t, tlw::string_t, tlw::table_t, tlw::cfunction_t>(false);
}
