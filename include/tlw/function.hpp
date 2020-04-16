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

    static inline void _func_failed(lua_State *L) {
        auto msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw std::runtime_error(msg);
    }

    template<typename ...>
    struct function : public reference {

    };

    template<>
    struct function<void()> : public reference {
        using fn_type = function<void()>;

        function(reference &&other) : reference(std::move(other)) {}

        void operator()() {
            stack_traits<function_t>::push(L, *this);
            if (lua_pcall(L, 0, 0, 0)) {
                _func_failed(L);
            }
        }
    };

    template<typename _r>
    struct function<_r()> : public reference {
        using fn_type = function<_r()>;

        function(reference &&other) : reference(std::move(other)) {}

        constexpr _r operator()() {
            stack_traits<function_t>::push(L, *this);
            if (lua_pcall(L, 0, 1, 0)) {
                _func_failed(L);
            }
            _r r = stack_traits<_r>::get(L, -1);
            lua_pop(L, 1);
            return r;
        }
    };

    template<typename ..._args>
    struct function<void(_args...)> : public reference {
        using fn_type = function<void(_args...)>;

        function(reference &&other) : reference(std::move(other)) {}

        constexpr void operator()(_args ...args) {
            stack_traits<function_t>::push(L, *this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 0, 0)) {
                _func_failed(L);
            }
            if constexpr (sizeof...(_args)) {
                lua_pop(L, sizeof...(_args));
            }
        }
    };

    template<typename _r, typename ..._args>
    struct function<_r(_args...)> : public reference {
        using fn_type = function<_r(_args...)>;

        function(reference &&other) : reference(std::move(other)) {}

        constexpr _r operator()(_args ...args) {
            stack_traits<function_t>::push(L, *this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 1, 0)) {
                _func_failed(L);
            }
            _r r = stack_traits<_r>::get(L, -1);
            lua_pop(L, 1);
            return r;
        }
    };
}

#endif //TLW_FUNCTION_HPP
