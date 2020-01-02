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

#ifndef TLW_FUNC_EXAMPLE_H
#define TLW_FUNC_EXAMPLE_H

namespace FunctionExample {
    inline static const char *script = "noop();\n"
                                       "return plus(2, 3)";

    inline static double plus(double a, double b) {
        return a + b;
    }

    inline static void noop() {

    }

    inline static lua_CFunction lplus = mk_function(plus);
    inline static lua_CFunction lnoop = mk_function(noop);

}

#endif //TLW_FUNC_EXAMPLE_H
