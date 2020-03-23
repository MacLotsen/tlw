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

#ifndef TLW_TYPES_H
#define TLW_TYPES_H

#include <lua.hpp>

namespace tlw {

    struct lua_struct_ref {
        constexpr static const int invalid = LUA_NOREF;
        lua_State *L;
        int r_idx;

        lua_struct_ref() : L(nullptr), r_idx(invalid) {}

        explicit lua_struct_ref(lua_State *L) : L(L), r_idx(luaL_ref(L, LUA_REGISTRYINDEX)) {}

        lua_struct_ref(const lua_struct_ref &other) noexcept : L(other.L) {
            if (other) {
                lua_rawgeti(L, LUA_REGISTRYINDEX, other.r_idx);
                r_idx = luaL_ref(L, LUA_REGISTRYINDEX);
            } else {
                r_idx = invalid;
            }
        }

        lua_struct_ref(lua_struct_ref &&other) noexcept : L(other.L), r_idx(std::exchange(other.r_idx, invalid)) {}

        ~lua_struct_ref() {
            if (*this) {
                luaL_unref(L, LUA_REGISTRYINDEX, r_idx);
            }
        }

        explicit operator bool() const noexcept {
            return r_idx != invalid;
        }
    };

    template<typename ...>
    struct lua_function : public lua_struct_ref {

    };

    template<typename _c_type, int _lua_type>
    struct lua_type_set {
        using type = _c_type;
        static constexpr int value = _lua_type;
    };

    using lua_nil_t =             lua_type_set<void *, LUA_TNIL>;
    using lua_none_t =            lua_type_set<void, LUA_TNONE>;
    using lua_bool_t =            lua_type_set<bool, LUA_TBOOLEAN>;
    using lua_number_t =          lua_type_set<lua_Number, LUA_TNUMBER>;
    using lua_string_t =          lua_type_set<const char *, LUA_TSTRING>;
    using lua_table_t =           lua_type_set<lua_struct_ref, LUA_TTABLE>;
    using lua_function_t =        lua_type_set<lua_struct_ref, LUA_TFUNCTION>;
    using lua_cfunction_t =       lua_type_set<lua_CFunction, LUA_TFUNCTION>;
    template<typename user_type>
    using lua_user_data_t =       lua_type_set<user_type, LUA_TUSERDATA>;
    template<typename user_type>
    using lua_light_user_data_t = lua_type_set<user_type, LUA_TLIGHTUSERDATA>;
    using lua_thread_t =          lua_type_set<lua_State *, LUA_TTHREAD>;

}

#endif //TLW_TYPES_H
