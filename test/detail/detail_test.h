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

#ifndef TLW_DETAIL_TEST_H
#define TLW_DETAIL_TEST_H

#include "../base_test.h"

class detail_test : public base_test {
protected:
    static inline const bool magic_bool = true;
    static inline const double magic_number = 5.5;
    static inline const char *magic_string = "TLW";
    static inline const auto magic_user_datum = tlw::example();
    static int noop(lua_State *) { return 0; }
};

#endif //TLW_DETAIL_TEST_H
