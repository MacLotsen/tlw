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

namespace tlw {

    template<typename ...>
    struct stack_traits {

    };

    template<typename _type>
    struct pop_traits {
        using st = stack_traits<_type>;

        static constexpr _type pop(lua_State *L) {
            auto &&value = st::get(L, -1);
            lua_pop(L, 1);
            return value;
        }
    };

    template<typename _lua_type>
    struct reference_stack_traits : public type_inspector<_lua_type> {
        using type_inspector<_lua_type>::inspect;
        static constexpr const bool is_lua_type = true;

        static constexpr void push(lua_State *L, typename _lua_type::type &value) {
            reference_traits<_lua_type>::push(L, value);
        }

        static constexpr typename _lua_type::type get(lua_State *L, int idx) {
            return reference_traits<_lua_type>::get(L, idx);
        }
    };

    template<>
    struct stack_traits<reference> : public reference_stack_traits<anonymous_t> {

    };

    template<>
    struct stack_traits<table_t> : public reference_stack_traits<table_t> {

    };

    template<>
    struct stack_traits<function_t> : public reference_stack_traits<function_t> {

    };


}

#endif //TLW_STACK_TRAITS_HPP
