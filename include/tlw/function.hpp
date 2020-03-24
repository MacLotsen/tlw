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

#ifndef TLW_FUNCTION_HPP
#define TLW_FUNCTION_HPP

#include <tlw/detail/stack_traits.hpp>

namespace tlw {

    template<typename ...>
    struct function : public struct_ref {

    };

    template<typename ..._args>
    struct function<void(_args...)> : struct_ref {
        void operator()(_args ...args) {
            stack_traits<struct_ref *>::push(L, this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 0, 0)) {
                throw std::runtime_error("Failed to call lua function.");
            }
            lua_settop(L, 0);
        }
    };

    template<typename _r, typename ..._args>
    struct function<_r(_args...)> : struct_ref {
        _r operator()(_args ...args) {
            stack_traits<struct_ref *>::push(L, this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 1, 0)) {
                throw std::runtime_error("Failed to call lua function.");
            }
            _r r = stack_traits<_r>::peek(L, -1);
            lua_pop(L, 1);
            return r;
        }
    };
}

#endif //TLW_FUNCTION_HPP
