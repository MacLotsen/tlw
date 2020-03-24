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

#ifndef TLW_STACK_TRAITS_HPP
#define TLW_STACK_TRAITS_HPP

#include <tlw/meta_table.hpp>

namespace tlw {

    template<typename ...>
    struct stack_traits {

    };

    template<typename _lua_type>
    struct primitive_stack_traits {
        static constexpr void push(lua_State *L, typename _lua_type::type value) {
            type_traits<_lua_type>::push(L, value);
        }

        static constexpr typename _lua_type::type peek(lua_State *L, int idx) {
            return type_traits<_lua_type>::peek(L, idx);
        }
    };

    template<>
    struct stack_traits<none_t::type> {
        static constexpr void push(lua_State *L) {
            type_traits<none_t>::push(L);
        }

        static constexpr none_t::type peek(lua_State *L, int idx) {
            return type_traits<none_t>::peek(L, idx);
        }
    };

    template<>
    struct stack_traits<nil_t::type> : public primitive_stack_traits<nil_t> {
    };

    template<>
    struct stack_traits<bool_t::type> : public primitive_stack_traits<bool_t> {
    };

    template<>
    struct stack_traits<number_t::type> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<float> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<int> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<long> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<string_t::type> : public primitive_stack_traits<string_t> {
    };

    template<class T>
    struct stack_traits<T> {
        using _user_data_t = user_data_t<T>;
        using _light_user_data_t = light_user_data_t<T>;

        static void push(lua_State *L, T value) {
            if (meta_table_t<typename _user_data_t::type>::name) {
                if constexpr (pointer_type<T>::valid) {
                    type_traits<_user_data_t>::push(L, value);
                } else {
                    type_traits<_user_data_t>::push(L, std::move(value));
                }
                if (luaL_newmetatable(L, meta_table_t<typename _user_data_t::type>::name)) {
                    // TODO create meta table
                }
                lua_setmetatable(L, -2);
            } else {
//                lua_type_traits<_light_user_data_t>()(L, value);
            }
        }

        static T peek(lua_State *L, int idx) {
            if (meta_table_t<T>::name) {
                return type_traits<_user_data_t>::peek(L, idx);
            } else {
//                return lua_type_traits<_light_user_data_t>()(L, idx);
            }
            return nullptr;
        }
    };
}

#endif //TLW_STACK_TRAITS_HPP
