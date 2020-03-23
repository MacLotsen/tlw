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

#ifndef TLW_TYPE_TRAITS_H
#define TLW_TYPE_TRAITS_H

#include <tlw/util.hpp>
#include "types.h"

namespace tlw {

    template<typename _lua_type_set>
    struct lua_type_inspector {
        using type = _lua_type_set;

        static constexpr bool inspect(lua_State *L) {
            return inspect(L, -1);
        }

        static bool inspect(lua_State *L, int idx) {
            return lua_type(L, idx) == type::value;
        }
    };

    template<typename _lua_type_set, class...>
    struct lua_type_traits {
        static typename _lua_type_set::type peek(lua_State *L, int idx) {
            throw std::runtime_error("unsupported type");
        }
        static void push(lua_State *L, typename _lua_type_set::type value) {
            throw std::runtime_error("unsupported type");
        }
    };

    template<typename _lua_type_set>
    struct lua_ref_traits {
        static typename _lua_type_set::type peek(lua_State *L, int idx) {
            lua_pushvalue(L, idx);
            auto r = lua_struct_ref(L);
            lua_pop(L, 1);
            return r;
        }

        static void push(lua_State *L, typename _lua_type_set::type value) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, value.r_idx);
        }
    };

    template<typename _lua_type_set, class ..._args>
    struct lua_ordered_type_traits : public lua_type_traits<_lua_type_set, _args...> {
        static constexpr typename _lua_type_set::type peek(lua_State *L) {
            return lua_type_traits<_lua_type_set, _args...>::peek(L, -1);
        }
    };

    template<>
    struct lua_type_traits<lua_none_t> {
        static constexpr typename lua_none_t::type peek(lua_State*, int) {}
        static constexpr void push(lua_State *) {}
    };

    template<>
    struct lua_type_traits<lua_nil_t> {
        static constexpr typename lua_nil_t::type peek(lua_State *L, int idx) {
            return nullptr;
        }

        static void push(lua_State *L, typename lua_nil_t::type) {
            lua_pushnil(L);
        }
    };

    template<>
    struct lua_type_traits<lua_bool_t> {
        static typename lua_bool_t::type peek(lua_State *L, int idx) {
            return lua_toboolean(L, idx) != 0;
        }
        static void push(lua_State *L, typename lua_bool_t::type value) {
            lua_pushboolean(L, value);
        }
    };

    template<>
    struct lua_type_traits<lua_number_t> {
        static typename lua_number_t::type peek(lua_State *L, int idx) {
            return lua_tonumber(L, idx);
        }

        static void push(lua_State *L, typename lua_number_t::type value) {
            lua_pushnumber(L, value);
        }
    };

    template<>
    struct lua_type_traits<lua_string_t> {
        static typename lua_string_t::type peek(lua_State *L, int idx) {
            return lua_tostring(L, idx);
        }

        static void push(lua_State *L, typename lua_string_t::type value) {
            lua_pushstring(L, value);
        }
    };

    template<>
    struct lua_type_traits<lua_table_t> : public lua_ref_traits<lua_table_t> {
        using lua_ref_traits<lua_table_t>::peek;
        using lua_ref_traits<lua_table_t>::push;
    };

    template<>
    struct lua_type_traits<lua_function_t> : public lua_ref_traits<lua_function_t> {
        using lua_ref_traits<lua_function_t>::peek;
        using lua_ref_traits<lua_function_t>::push;
    };

    template<class _user_type>
    struct lua_type_traits<lua_user_data_t<_user_type>> {
        static _user_type peek(lua_State *L, int idx) {
            auto user_data = (_user_type *) lua_touserdata(L, idx);
            return *user_data;
        }

        static void push(lua_State *L, _user_type value) {
            auto user_data = (_user_type *) lua_newuserdata(L, sizeof(_user_type *));
            if constexpr (pointer_type<_user_type>::valid) {
                *user_data = value;
            } else {
                *user_data = std::move(value);
            }
        }
    };
}

#endif //TLW_TYPE_TRAITS_H
