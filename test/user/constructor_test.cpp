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
    auto expose = tlw::define<tlw::example>("example")
            .ctor<>()
            .build();
    s.push(expose(L));
    lua_getfield(L, -1, "new");
    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << "Failed to call constructor";
    }

    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::get(L, -1);
    delete created_example;

    // Pop trailing values
    lua_pop(L, 2);
}

TEST_F(user_test, test_constructor_with_parameters) {
    auto expose = tlw::define<tlw::example>("example")
            .ctor<float>()
            .build();
    s.push(expose(L));
    lua_getfield(L, -1, "new");
    s.push(5.5);

    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::get(L, -1);
    delete created_example;

    // Pop trailing value
    lua_pop(L, 2);
}

TEST_F(user_test, test_multi_constructor) {
    auto ref = tlw::define<tlw::example>("example")
            .ctor<>()
            .ctor<float>()
            .build()(L);
    s.push(ref);
    lua_getfield(L, -1, "new");
    s.push(5.5);
    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::type_traits<tlw::lua_example_t>::get(L, -1);
    ASSERT_EQ(5.5, created_example->val);
    delete created_example;
    // Pop the error message
    lua_pop(L, 2);

    s.push(ref);
    lua_getfield(L, -1, "new");
    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::type_inspector<tlw::lua_example_t>::inspect(L));
    created_example = tlw::type_traits<tlw::lua_example_t>::get(L, -1);
    ASSERT_EQ(tlw::example::invalid, created_example->val);
    delete created_example;
    // Pop the error message
    lua_pop(L, 2);


    s.push(ref);
    lua_getfield(L, -1, "new");
    s.push(5.5);
    s.push("a string");
    if (!lua_pcall(L, 2, 1, 0)) {
        FAIL() << "No such constructor was defined for float, string.";
    }
    // Pop the error message
    lua_pop(L, 2);
}
