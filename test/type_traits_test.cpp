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

TEST_F(detail_test, test_peek) {
    lua_pushboolean(L, magic_bool);
    ASSERT_EQ(magic_bool, tlw::ordered_type_traits<tlw::bool_t>::peek(L));

    lua_pushnumber(L, magic_number);
    ASSERT_EQ(magic_number, tlw::ordered_type_traits<tlw::number_t>::peek(L));

    lua_pushstring(L, magic_string);
    ASSERT_STREQ(magic_string, tlw::ordered_type_traits<tlw::string_t>::peek(L));

    lua_createtable(L, 0, 0);
    auto t_ref = tlw::ordered_type_traits<tlw::table_t>::peek(L);
    ASSERT_TRUE(static_cast<bool>(t_ref));

    lua_pushcfunction(L, &noop);
    auto fn_ref = tlw::ordered_type_traits<tlw::function_t>::peek(L);
    ASSERT_TRUE(static_cast<bool>(fn_ref));

    auto user_data = (const tlw::example **) lua_newuserdata(L, sizeof(const tlw::example **));
    *user_data = &magic_user_datum;
    ASSERT_EQ(&magic_user_datum, tlw::ordered_type_traits<tlw::lua_example_t>::peek(L));
}

TEST_F(detail_test, test_push) {
    tlw::type_traits<tlw::bool_t>::push(L, magic_bool);
    ASSERT_EQ(magic_bool, lua_toboolean(L, -1));

    tlw::type_traits<tlw::number_t>::push(L, magic_number);
    ASSERT_EQ(magic_number, lua_tonumber(L, -1));

    tlw::type_traits<tlw::string_t>::push(L, magic_string);
    ASSERT_STREQ(magic_string, lua_tostring(L, -1));

    lua_createtable(L, 0, 0);
    auto t_ref = tlw::struct_ref(L);
    tlw::type_traits<tlw::table_t>::push(L, std::move(t_ref));
    ASSERT_EQ(tlw::table_t::value, lua_type(L, -1));
    ASSERT_FALSE(static_cast<bool>(t_ref));

    lua_pushcfunction(L, &noop);
    auto fn_ref = tlw::struct_ref(L);
    tlw::type_traits<tlw::function_t>::push(L, std::move(fn_ref));
    ASSERT_EQ(tlw::function_t::value, lua_type(L, -1));
    ASSERT_FALSE(static_cast<bool>(fn_ref));

    tlw::type_traits<tlw::lua_example_t>::push(L, &magic_user_datum);
    ASSERT_EQ(&magic_user_datum, *(tlw::example **) lua_touserdata(L, -1));
}
