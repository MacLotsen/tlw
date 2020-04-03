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

#include <tlw/detail/type_traits.hpp>
#include <tlw/function.hpp>
#include <tlw/meta_table.hpp>
#include <utility>

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

    template<typename _lua_type>
    struct struct_stack_traits {
        static void push(lua_State *L, typename _lua_type::type value) {
            ref_traits<_lua_type>::push(L, std::move(value));
        }

        static typename _lua_type::type peek(lua_State *L, int idx) {
            return ref_traits<_lua_type>::peek(L, idx);
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
    struct stack_traits<double> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<const double> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<float> : public primitive_stack_traits<number_t> {
    };

    template<>
    struct stack_traits<const float> : public primitive_stack_traits<number_t> {
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

    template<>
    struct stack_traits<cfunction_t::type> : public primitive_stack_traits<cfunction_t> {
    };

    template<>
    struct stack_traits<table_t> : public struct_stack_traits<table_t> {

    };

    template<class _type>
    struct stack_traits<_type> {
        using _base_type = typename pointer_type<_type>::value_type;
        using _user_data_t = user_data_t<_type>;
        using _light_user_data_t = light_user_data_t<_type>;

        static void push(lua_State *L, _type value) {
            if (meta_table_registry<_base_type>::name) {
                if constexpr (pointer_type<_type>::valid) {
                    type_traits<_user_data_t>::push(L, value);
                } else {
                    type_traits<_user_data_t>::push(L, std::move(value));
                }
                luaL_setmetatable(L, meta_table_registry<_base_type>::name);
            } else {
                type_traits<_light_user_data_t>::push(L, value);
            }
        }

        static _type peek(lua_State *L, int idx) {
            if (meta_table_registry<_base_type>::name) {
                return type_traits<_user_data_t>::peek(L, idx);
            } else {
                return type_traits<_light_user_data_t>::peek(L, idx);
            }
        }
    };
}

#endif //TLW_STACK_TRAITS_HPP
