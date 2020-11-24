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


TEST_F(user_test, test_move_value) {
    tlw::define<tlw::example>("example_value")
            .build();
    tlw::example value = tlw::example(5);
    s.push(std::move(value));
    ASSERT_FALSE(tlw::type_inspector<tlw::nil_t>::inspect(L));
    ASSERT_TRUE(tlw::type_inspector<tlw::user_data_t<const tlw::example *>>::inspect(L));

    // Pop trailing value
    lua_pop(L, 1);
    ASSERT_EQ(0, tlw::example_tracker::copied);
    ASSERT_EQ(1, tlw::example_tracker::moved);
    ASSERT_EQ(1, tlw::example_tracker::deleted);
}

TEST_F(user_test, test_inner_value_move) {
    luaopen_base(L);
    lua_settop(L, 0);

    tlw::define<tlw::example>("example")
            .build()(L);

    ASSERT_EQ(0, lua_gettop(L));
    auto example = tlw::example(5.5);
    s.push(std::move(example));
    lua_setglobal(L, "example1");

    if (luaL_loadstring(L, "example1 = nil\ncollectgarbage()")) {
        FAIL() << "Failed to load script '" << lua_tostring(L, -1) << "'";
    }

    auto script = s.pop<tlw::function<void()>>();
    script();

    ASSERT_EQ(0, tlw::example_tracker::copied);
    ASSERT_EQ(1, tlw::example_tracker::moved);
    ASSERT_EQ(1, tlw::example_tracker::deleted);
}
