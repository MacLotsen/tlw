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

#ifndef TLW_USER_TRAITS_HPP
#define TLW_USER_TRAITS_HPP

#include <tlw/meta_table.hpp>

namespace tlw {

    template<class _type>
    struct user_type_traits {

        static bool inspect(lua_State *L, int idx) {
            int type_id = lua_type(L, idx);
            switch(lua_type(L, idx)) {
                case LUA_TUSERDATA:
                    return inspect_meta_table(L, idx);
                case LUA_TLIGHTUSERDATA:
                    return meta_table<_type>::name == nullptr;
                default:
                    return false;
            }
        }

        static bool inspect_meta_table(lua_State *L, int idx) {
            int top = lua_gettop(L);
            lua_getmetatable(L, idx);
            lua_getfield(L, -1, "__name");
            bool matches = strcmp(lua_tostring(L, -1), meta_table<_type>::name) == 0;
            lua_settop(L, top);
            return matches;
        }
    };

    template<class _type>
    struct stack_traits<const _type&> : public user_type_traits<_type> {
        using user_type_traits<_type>::inspect;
        using _base_type = typename cpp_type<_type>::value_type;
        using _user_data_t = user_data_t<_type>;
        using _light_user_data_t = light_user_data_t<_type>;

        static void push(lua_State *L, const _type &value) {
            if (meta_table_registry<_type>::name) {
                    type_traits<_user_data_t>::push(L, value);
                luaL_getmetatable(L, meta_table_registry<_type>::name);
                lua_setmetatable(L, -2);
            } else {
                throw std::runtime_error("Cannot push a const reference as light user datum.");
            }
        }

        static const _type& get(lua_State *L, int idx) {
            if (meta_table_registry<_type>::name) {
                return type_traits<_user_data_t>::get(L, idx);
            } else {
                throw std::runtime_error("Cannot get a const reference as light user datum.");
            }
        }
    };

    template<class _type>
    struct stack_traits<_type> : public user_type_traits<_type> {
        using user_type_traits<_type>::inspect;
        using _base_type = typename cpp_type<_type>::value_type;
        using _user_data_t = user_data_t<_type>;
        using _light_user_data_t = light_user_data_t<_type>;

        static void push(lua_State *L, _type value) {
            if (meta_table_registry<_type>::name) {
                if constexpr (cpp_type<_type>::is_pointer) {
                    type_traits<_user_data_t>::push(L, value);
                } else {
                    type_traits<_user_data_t>::push(L, std::move(value));
                }
                luaL_getmetatable(L, meta_table_registry<_type>::name);
                lua_setmetatable(L, -2);
            } else {
                type_traits<_light_user_data_t>::push(L, value);
            }
        }

        static _type get(lua_State *L, int idx) {
            if (meta_table_registry<_type>::name) {
                return type_traits<_user_data_t>::get(L, idx);
            } else {
                return type_traits<_light_user_data_t>::get(L, idx);
            }
        }
    };

}

#endif //TLW_USER_TRAITS_HPP
