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
    tlw::state L = tlw::state(_L);
    load_entity(L);
    load_vec4(L);
    load_mat4(L);
    return 0;
}

}
