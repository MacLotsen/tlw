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
    struct stack_traits<function <void()>> : public reference_stack_traits <function_t> {
    using type = function<void()>;
    using reference_stack_traits<function_t>::push;

    static inline type get(lua_State *L, int idx) {
        return std::move(type(std::move(reference_stack_traits::get(L, idx))));
    }

};

    template<typename _r>
    struct stack_traits<function < _r()>> : public reference_stack_traits <function_t> {
        using type = function<_r()>;
        using reference_stack_traits<function_t>::push;

        static inline type get(lua_State *L, int idx) {
            return std::move(type(std::move(reference_stack_traits::get(L, idx))));
        }

    };

    template<typename _r, typename ..._args>
    struct stack_traits<function < _r(_args...)>> : public reference_stack_traits <function_t> {
        using type = function<_r(_args...)>;
        using reference_stack_traits<function_t>::push;

        static inline type get(lua_State *L, int idx) {
            return std::move(type(std::move(reference_stack_traits::get(L, idx))));
        }
    };

}

#endif //TLW_FUNCTION_TRAITS_HPP
