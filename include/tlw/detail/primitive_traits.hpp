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

#ifndef TLW_PRIMITIVE_TRAITS_HPP
#define TLW_PRIMITIVE_TRAITS_HPP

#include <array>
#include <vector>
#include <unordered_map>

namespace tlw {

    template<typename _lua_type, typename _r = typename _lua_type::type>
    struct primitive_stack_traits : public type_inspector<_lua_type> {
        static constexpr const bool is_lua_type = true;
        using type_inspector<_lua_type>::inspect;

        static constexpr void push(lua_State *L, typename _lua_type::type value) {
            type_traits<_lua_type>::push(L, value);
        }

        static constexpr _r get(lua_State *L, int idx) {
            return type_traits<_lua_type>::get(L, idx);
        }
    };

    template<>
    struct stack_traits<none_t::type> : public type_inspector<none_t> {
        static constexpr void push(lua_State *L) {
            type_traits<none_t>::push(L);
        }

        static constexpr none_t::type get(lua_State *L, int idx) {
            return type_traits<none_t>::get(L, idx);
        }
    };

    template<>
    struct stack_traits<nil_t::type> : public primitive_stack_traits<nil_t> {
    };

    template<>
    struct stack_traits<bool_t::type> : public primitive_stack_traits<bool_t> {
    };

    template<>
    struct stack_traits<double> : public primitive_stack_traits<number_t, double> {
    };

    template<>
    struct stack_traits<const double> : public primitive_stack_traits<number_t, const double> {
    };

    template<>
    struct stack_traits<float> : public primitive_stack_traits<number_t, float> {
    };

    template<>
    struct stack_traits<const float> : public primitive_stack_traits<number_t, const float> {
    };

    template<>
    struct stack_traits<int> : public primitive_stack_traits<integer_t, int> {
    };

    template<>
    struct stack_traits<unsigned int> : public primitive_stack_traits<integer_t, unsigned int> {
    };

    template<>
    struct stack_traits<long> : public primitive_stack_traits<integer_t, long> {
    };

    template<>
    struct stack_traits<unsigned long> : public primitive_stack_traits<integer_t, unsigned long> {
    };

    template<>
    struct stack_traits<string_t::type> : public primitive_stack_traits<string_t> {
    };

    template<>
    struct stack_traits<std::string> : public primitive_stack_traits<string_t, std::string> {
        using st = primitive_stack_traits<string_t, std::string>;

        static void push(lua_State *L, const std::string &value) {
            st::push(L, value.c_str());
        }

        static std::string get(lua_State *L, int idx) {
            return std::string(type_traits<string_t>::get(L, idx));
        }
    };

    template<>
    struct stack_traits<cfunction_t::type> : public primitive_stack_traits<cfunction_t> {
    };
}

#endif //TLW_PRIMITIVE_TRAITS_HPP
