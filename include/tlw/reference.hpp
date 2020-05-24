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

#ifndef TLW_REFERENCE_HPP
#define TLW_REFERENCE_HPP

#include <tlw/state.hpp>
#include <utility>

namespace tlw {

    struct reference {
        static constexpr const int invalid = LUA_NOREF;

        state L;
        int r_idx;

        reference() : L(nullptr), r_idx(invalid) {}

        reference(const state &L) : L(L), r_idx(luaL_ref(L, LUA_REGISTRYINDEX)) {}

        reference(const state &L, int idx) : L(L) {
            lua_pushvalue(L, idx);
            r_idx = luaL_ref(L, LUA_REGISTRYINDEX);
        }

        reference(const reference &other) noexcept: L(other.L) {
            if (other) {
                lua_rawgeti(L, LUA_REGISTRYINDEX, other.r_idx);
                r_idx = luaL_ref(L, LUA_REGISTRYINDEX);
            } else {
                r_idx = invalid;
            }
        }

        reference(reference &&other) noexcept: L(std::move(other.L)), r_idx(std::exchange(other.r_idx, invalid)) {}

        ~reference() {
            if (*this) {
                luaL_unref(L, LUA_REGISTRYINDEX, r_idx);
            }
        }

        reference &operator=(const reference &) = default;

        constexpr explicit operator bool() const noexcept {
            return r_idx != invalid && r_idx != LUA_REFNIL;
        }

        bool operator==(const reference &other) const {
            return L.operator lua_State *() == other.L.operator lua_State *() &&
                   r_idx == other.r_idx;
        }

        bool operator!=(const reference &other) const {
            return !(*this == other);
        }
    };

}

#endif //TLW_REFERENCE_HPP
