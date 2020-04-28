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

#ifndef TLW_TABLE_TRAITS_H
#define TLW_TABLE_TRAITS_H

namespace tlw {

    template<typename _type, typename _key>
    struct table_traits {
        using key_traits = stack_traits<_key>;
        using value_traits = stack_traits<_type>;

        static constexpr _type get(lua_State *L, int idx, _key k) {
            key_traits::push(L, k);
            lua_gettable(L, idx);
            _type val = value_traits::get(L, -1);
            lua_pop(L, 1);
            if constexpr (cpp_type<_type>::is_movable) {
                return std::move(val);
            } else {
                return val;
            }
        }

        static constexpr void set(lua_State *L, int idx, _key k, _type val) {
            key_traits::push(L, k);
            value_traits::push(L, val);
            lua_settable(L, idx);
        }
    };

}

#endif //TLW_TABLE_TRAITS_H
