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

TEST_F(user_test, test_property) {
    tlw::define<tlw::example>("example")
            .prop("val", &tlw::example::val)
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    tlw::example e(5.5);
    s.push(&e);
    lua_setglobal(L, "example1");

    if (luaL_dostring(L, "return example1.val")) {
        FAIL() << "Failed to execute getter script '" << lua_tostring(L, -1) << "'";
    }

    ASSERT_EQ(5.5, s.pop<float>());

    if (luaL_dostring(L, "example1.val = 4.5")) {
        FAIL() << "Failed to execute setter script '" << lua_tostring(L, -1) << "'";
    }

    ASSERT_EQ(4.5, e.val);

    e.val = 5.5;
    s.push<const tlw::example>(e);
    lua_setglobal(L, "example1");

    if (luaL_dostring(L, "return example1.val")) {
        FAIL() << "Failed to execute getter script '" << lua_tostring(L, -1) << "'";
    }

    ASSERT_EQ(5.5, s.pop<float>());

    if (luaL_dostring(L, "example1.val = 4.5")) {
        FAIL() << "Failed to execute setter script '" << lua_tostring(L, -1) << "'";
    }
}

TEST_F(user_test, test_property_with_const_ud) {
    tlw::define<tlw::example>("example")
            .prop("val", &tlw::example::val)
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    const tlw::example e(5.5);
    s.push(&e);
    lua_setglobal(L, "example1");

    if (luaL_dostring(L, "return example1.val")) {
        FAIL() << "Failed to execute script '" << lua_tostring(L, -1) << "'";
    }

    ASSERT_EQ(5.5, s.pop<float>());

    if (!luaL_dostring(L, "example1.val = 4.5")) {
        FAIL() << "Script must throw an error since it's const (read only)";
    }

    // Remove error message
    lua_pop(L, 1);
}

TEST_F(user_test, test_const_property) {
    tlw::define<tlw::example>("example")
            .prop("val", &tlw::example::initial)
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    tlw::example e(5.5);
    s.push(&e);
    lua_setglobal(L, "example1");

    if (luaL_dostring(L, "return example1.val")) {
        FAIL() << "Failed to execute script '" << lua_tostring(L, -1) << "'";
    }

    ASSERT_EQ(5.5, s.pop<float>());

    if (!luaL_dostring(L, "example1.val = 4.5")) {
        FAIL() << "Script must throw an error since it's const (read only)";
    }

    // Remove error message
    lua_pop(L, 1);
}
