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

#ifndef TLW_STACK_HPP
#define TLW_STACK_HPP

#include <tlw/state.hpp>
#include <tlw/detail/stack_traits.hpp>
#include <tlw/detail/function_traits.hpp>
#include <utility>

namespace tlw {

    class stack {
        state L;
    public:
        explicit stack() noexcept : stack(state::invalid_state()) {}

        explicit stack(state L) noexcept : L(std::move(L)) {}

        stack(stack &&) = delete;

        stack(const stack &other) noexcept {
            if (other) {
                L = other.L;
            }
        }

        stack &operator=(const stack &other) noexcept {
            if (other) {
                L = other.L;
            }
            return *this;
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(L);
        }

        template<class T>
        constexpr void push(T value) {
            stack_traits<T>::push(L, std::move(value));
        }

        template<class T>
        constexpr T get(int idx) {
            return stack_traits<T>::peek(L, idx);
        }

        template<class T>
        constexpr T pop() {
            T val = get<T>(-1);
            lua_pop(L, 1);
            return val;
        }

        template<class T>
        constexpr T pick(int idx) {
            T val = get<T>(idx);
            lua_remove(L, idx);
            return val;
        }

        template<typename ...Ts>
        constexpr void place(Ts ...values) {
            (..., push<Ts>(values));
        }

        template<class ...Ts>
        constexpr std::tuple<Ts...> grab() {
            int begin = lua_gettop(L) + 1 - sizeof...(Ts);
            int idx = begin;
            auto values = std::tuple{ordered_get<Ts>(idx)...};
            lua_settop(L, begin - 1);
            return values;
        }

    private:

        template<class T>
        constexpr T ordered_get(int &idx) {
            return get<T>(idx++);
        }

    };
}

#endif //TLW_STACK_HPP
