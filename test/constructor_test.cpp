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

TEST_F(user_test, test_constructor) {
    tlw::define<tlw::example>("example")
            .ctor<>()
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    lua_getglobal(L, "example");
    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << "Failed to call constructor";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::peek(L, -1);
    delete created_example;
}

TEST_F(user_test, test_constructor_with_parameters) {
    tlw::define<tlw::example>("example")
            .ctor<float>()
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    lua_getglobal(L, "example");
    s.push(5.5);
    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::peek(L, -1);
    delete created_example;
}

TEST_F(user_test, test_multi_constructor) {
    tlw::define<tlw::example>("example")
            .ctor<>()
            .ctor<float>()
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    lua_getglobal(L, "example");
    s.push(5.5);
    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::peek(L, -1);
    ASSERT_EQ(5.5, created_example->val);
    delete created_example;
    lua_getglobal(L, "example");
    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    created_example = tlw::type_traits<tlw::lua_example_t>::peek(L, -1);
    ASSERT_EQ(tlw::example::invalid, created_example->val);
    delete created_example;
}

TEST_F(user_test, test_destructor) {
    luaopen_base(L);
    lua_settop(L, 0);

    tlw::define<tlw::example>("example")
            .dtor()
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);

    ASSERT_EQ(0, lua_gettop(L));
    const tlw::example *example = new tlw::example(5.5);
    s.push(example);
    lua_setglobal(L, "example1");

    if (luaL_loadstring(L, "example1 = nil\ncollectgarbage()")) {
        FAIL() << "Failed to load script '" << lua_tostring(L, -1) << "'";
    }

    auto script = s.pop<tlw::function<void()>>();
    script();

    ASSERT_EQ(1, tlw::example_tracker::deleted);
}
