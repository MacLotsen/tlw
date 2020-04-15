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

#include <tlw/detail/stack_traits.hpp>
#include <tlw/detail/table_traits.hpp>
#include <tlw/any.hpp>

namespace tlw {

    template<bool is_global>
    struct _table {
    };

    using table = _table<false>;
    using global_table = _table<true>;

    template<bool is_global>
    struct stack_traits<_table<is_global>> : public reference_stack_traits<table_t> {

    };

    template<>
    struct _table<true> {
        state L;

        _table(state L) : L(std::move(L)) {}

        template<typename _type, typename _key>
        _type get(_key k) {
            return table_traits<_type, _key>::get(L, LUA_GLOBALSINDEX, k);
        }

        template<typename _type, typename _key>
        void set(_key k, _type val) {
            table_traits<_type, _key>::set(L, LUA_GLOBALSINDEX, k, std::move(val));
        }

        template<typename _key>
        any operator[](_key k) {
            lua_getglobal(L, k);
            auto val = stack_traits<any>::get(L, -1);
            lua_pop(L, 1);
            return val;
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
        _type get(_key k) {
            traits::push(L, *this);
            auto value = table_traits<_type, _key>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return value;
        }

        template<typename _type, typename _key>
        void set(_key k, _type val) {
            traits::push(L, *this);
            table_traits<_type, _key>::set(L, lua_gettop(L), k, std::move(val));
            lua_pop(L, 1);
        }

        template<typename _key>
        any operator[](_key k) {
            traits::push(L, *this);
            auto val = table_traits<any, _key>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return val;
        }
    };

}

#endif //TLW_TABLE_HPP
