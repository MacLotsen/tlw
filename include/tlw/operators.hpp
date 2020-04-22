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

#ifndef TLW_OPERATORS_HPP
#define TLW_OPERATORS_HPP

namespace tlw {

    template<typename _user_type, typename ...>
    struct __operator {

        static constexpr int to_string(lua_State *L) {
            // Clear the ud from the stack
            lua_settop(L, 0);
            lua_pushstring(L, meta_table<_user_type>::name);
            return 1;
        }

        // not generic
        static constexpr int call(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        // not generic
        static constexpr int len(lua_State *L) {
            lua_pushnumber(L, 1);
            return 1;
        }

        // not generic
        static constexpr int pairs(lua_State *L) {
            lua_pushnumber(L, 1);
            return 1;
        }

        // not generic
        static constexpr int ipairs(lua_State *L) {
            lua_pushnumber(L, 1);
            return 1;
        }

        static constexpr int metatable(lua_State *L) {
            lua_pushstring(L, "private");
            return 1;
        }

    };

    template<typename _user_type, typename _other_type>
    struct __operator<_user_type, _other_type> {

        static constexpr int unm(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int add(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int sub(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int mul(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int div(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int mod(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int pow(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int concat(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int eq(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int lt(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int le(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

    };

}

#endif //TLW_OPERATORS_HPP
