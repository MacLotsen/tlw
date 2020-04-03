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

static const char * script = "e.print()";

TEST_F(user_test, test_const_method) {
    tlw::define<tlw::example>("example")
            .method("print", &tlw::example::print)
            .finish();
    tlw::meta_table_registry<tlw::example>::expose(L);
    const tlw::example e(5.5);
    s.push(&e);
    lua_setglobal(L, "e");

    if (luaL_dostring(L, script)) {
        FAIL() << "Failed to execute script '" << lua_tostring(L, -1) << "'";
    }
}
