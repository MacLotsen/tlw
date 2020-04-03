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

    template<typename _r, typename ..._args>
    struct stack_traits<function<_r(_args...)>> : public struct_stack_traits<function_t> {
        using fn_t = function<_r(_args...)>;
        using struct_stack_traits::push;

        constexpr static fn_t peek(lua_State *L, int idx) {
            return fn_t(std::move(struct_stack_traits::peek(L, idx)));
        }
    };

}

#endif //TLW_FUNCTION_TRAITS_HPP
