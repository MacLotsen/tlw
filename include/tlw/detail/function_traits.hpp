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

#ifndef TLW_FUNCTION_TRAITS_HPP
#define TLW_FUNCTION_TRAITS_HPP

#include <tlw/detail/stack_traits.hpp>
#include <tlw/function.hpp>

namespace tlw {

    template<>
    struct stack_traits<function<void()>> : public reference_stack_traits<function_t> {
        using type = function<void()>;
        using reference_stack_traits<function_t>::push;

        static type get(lua_State *L, int idx) {
            return type(reference_traits<function_t>::get(L, idx));
        }

    };

    template<typename _r>
    struct stack_traits<function<_r()>> : public reference_stack_traits<function_t> {
        using type = function<_r()>;
        using reference_stack_traits<function_t>::push;

        static type get(lua_State *L, int idx) {
            return type(reference_traits<function_t>::get(L, idx));
        }

    };

    template<typename _r, typename ..._args>
    struct stack_traits<function<_r(_args...)>> : public reference_stack_traits<function_t> {
        using type = function<_r(_args...)>;
        using reference_stack_traits<function_t>::push;

        static type get(lua_State *L, int idx) {
            return type(reference_traits<function_t>::get(L, idx));
        }
    };

    template<typename _r, typename ..._args>
    struct stack_traits<_r(*)(_args...)> {
        using type = _r(*)(_args...);
        static constexpr const bool is_lua_type = false;

        static constexpr void push(lua_State *L, type fn) {
            lua_pushlightuserdata(L, (void*) fn);
            lua_pushcclosure(L, __wrap, 1);
        }

        static constexpr int __wrap(lua_State *L) {
            return __wrap(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr int __wrap(lua_State *L, seq<Is...>) {
            auto fn = (type) lua_touserdata(L, lua_upvalueindex(1));
            _r &&ret = fn(stack_traits<_args>::get(L, Is + 1)...);
            lua_settop(L, 0);
            stack_traits<_r>::push(L, ret);
            return 1;
        }
    };

    template<typename ..._args>
    struct stack_traits<void(*)(_args...)> {
        using type = void(*)(_args...);
        static constexpr const bool is_lua_type = false;

        static constexpr void push(lua_State *L, type fn) {
            lua_pushlightuserdata(L, (void*) fn);
            lua_pushcclosure(L, __wrap, 1);
        }

        static constexpr int __wrap(lua_State *L) {
            return __wrap(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr int __wrap(lua_State *L, seq<Is...>) {
            auto fn = (type) lua_touserdata(L, lua_upvalueindex(1));
            fn(stack_traits<_args>::get(L, Is + 1)...);
            lua_settop(L, 0);
            return 0;
        }
    };

}

#endif //TLW_FUNCTION_TRAITS_HPP
