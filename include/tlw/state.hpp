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

#ifndef TLW_STATE_HPP
#define TLW_STATE_HPP

#include <lua.hpp>

namespace tlw {

    struct state_holder {
        lua_State *L;
        long ref_count;
    };

    class state {

        static constexpr inline state_holder * const invalid = nullptr;

        state_holder* holder;

        constexpr explicit state (bool) noexcept : holder(nullptr) {
        }

    public:

        explicit state() noexcept : holder(new state_holder {luaL_newstate(), 1}) {
        }

        explicit state(lua_State *L) noexcept : holder(new state_holder{L, -1}) {
        }

        state(const state &other) noexcept {
            if (other) {
                holder = other.holder;
                if (holder->ref_count > 0)
                    holder->ref_count++;
                else
                    holder->ref_count--;
            } else {
                holder = state::invalid;
            }
        }

        state(state &&other) noexcept {
            if (other) {
                holder = other.holder;
                other.holder = state::invalid;
            } else {
                holder = state::invalid;
            }
        }

        ~state() noexcept {
            invalidate();
        }

        constexpr state & operator =(state const & other) noexcept {
            invalidate();
            if (other) {
                holder = other.holder;
                if (holder->ref_count > 0)
                    holder->ref_count++;
                else
                    holder->ref_count--;
            } else {
                holder = nullptr;
            }
            return *this;
        }

        constexpr state & operator =(state &&other) noexcept {
            invalidate();
            holder = other.holder;
            other.holder = state::invalid;
            return *this;
        }

        constexpr explicit operator bool() const {
            return holder;
        }

        constexpr operator lua_State * () const {
            if (!holder)
                throw std::runtime_error("invalid lua state");
            return holder->L;
        }

        static state invalid_state() {
            return state(true);
        }

    private:

        constexpr void invalidate() noexcept {
            if (*this) {
                bool is_owner = holder->ref_count > 0;

                if (is_owner)
                    holder->ref_count--;
                else
                    holder->ref_count++;

                if (holder->ref_count == 0) {
                    if (is_owner)
                        lua_close(holder->L);
                    delete holder;
                }
            }
        }
    };

}

#endif //TLW_STATE_HPP
