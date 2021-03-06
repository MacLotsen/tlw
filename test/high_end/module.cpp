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

#include "high_end_examples.h"

extern "C" {

static int wrap_exceptions(lua_State *L, lua_CFunction f) {
    try {
        return f(L);  // Call wrapped function and return result.
    } catch (const char *s) {  // Catch and convert exceptions.
        lua_pushstring(L, s);
    } catch (std::exception& e) {
        lua_pushstring(L, e.what());
    } catch (...) {
        lua_pushliteral(L, "caught (...)");
    }
    return lua_error(L);  // Rethrow as a Lua error.
}

/**
 * To run lua jit using this module:
 *
 * `$ luajit -l libtlw-examplelib`
 *
 * or load it later in interactive mode
 *
 * `$ luajit`
 * `> require 'libtlw-examplelib'`
 *
 * entity, vec4 and mat4 will directly be available in the global namespace
 *
 */
int luaopen_examplelib(lua_State *_L) {
//    lua_pushlightuserdata(_L, (void *)wrap_exceptions);
//    luaJIT_setmode(_L, -1, LUAJIT_MODE_WRAPCFUNC|LUAJIT_MODE_ON);
    tlw::state L = tlw::state(_L);
    tlw::lua lua = tlw::lua(L);
    lua["entity"] = tlw::load_entity(L);
    lua["vec4"] = tlw::load_vec4(L);
    lua["mat4"] = tlw::load_mat4(L);

    lua["namespace"] = lua.table();
    lua["namespace"]["vec4"] = tlw::load_vec4(L);

    return 0;
}

}
