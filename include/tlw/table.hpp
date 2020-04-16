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

#ifndef TLW_TABLE_HPP
#define TLW_TABLE_HPP

#include <tlw/_table.hpp>
#include <tlw/detail/stack_traits.hpp>
#include <tlw/detail/table_traits.hpp>
#include <tlw/table_reference.hpp>

namespace tlw {

    template<>
    struct _table<true> {
        state L;

        _table(state L) : L(std::move(L)) {}

        template<typename _type, typename _key>
        constexpr _type get(_key k) {
            return table_traits<_type, _key>::get(L, LUA_GLOBALSINDEX, k);
        }

        template<typename _type, typename _key>
        constexpr void set(_key k, _type val) {
            table_traits<_type, _key>::set(L, LUA_GLOBALSINDEX, k, std::move(val));
        }

        template<typename _key>
        constexpr table_reference<_table<true>, _key> operator[](_key k) {
            auto val = get<reference>(k);
            return table_reference<_table<true>, _key>(std::move(val), *this, k);
        }
    };

    template<>
    struct _table<false> : public reference {
        using traits = stack_traits<_table<true>>;

        _table(const reference &other) : reference(other) {

        }

        _table(reference &&other) : reference(std::move(other)) {

        }

        template<typename _type, typename _key>
        constexpr _type get(_key k) {
            traits::push(L, *this);
            auto value = table_traits<_type, _key>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return value;
        }

        template<typename _type, typename _key>
        constexpr void set(_key k, _type val) {
            traits::push(L, *this);
            table_traits<_type, _key>::set(L, lua_gettop(L), k, std::move(val));
            lua_pop(L, 1);
        }

        template<typename _key>
        constexpr table_reference<_table<false>, _key> operator[](_key k) {
            return table_reference<_table<false>, _key>(get<reference>(k), *this, k);
        }
    };

}

#endif //TLW_TABLE_HPP
