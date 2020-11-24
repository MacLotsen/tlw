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

#ifndef TLW_TABLE_REFERENCE_HPP
#define TLW_TABLE_REFERENCE_HPP

#include <lua.hpp>
#include <tlw/_table.hpp>
#include <tlw/reference.hpp>
#include <tlw/detail/stack_traits.hpp>

namespace tlw {

    template<typename _table_type, typename _key>
    struct table_reference : public reference {
        _table_type origin;
        _key at;

        table_reference(const reference &other, const _table_type& origin, _key at) : reference(other), origin(origin), at(at) {

        }

        table_reference(reference &&other, _table_type &&origin, _key at) : reference(std::move(other)), origin(std::move(origin)), at(at) {

        }

        template<typename _type>
        constexpr operator _type() {
            using traits = stack_traits<_type>;
            stack_traits<table_reference<_table_type, _key>>::push(L, *this);
            auto val = traits::get(L, -1);
            lua_pop(L, 1);
            return val;
        }

        template<typename _key2>
        constexpr table_reference<_table<false>, _key2> operator[](_key2 k) {
            stack_traits<table_reference>::push(L, *this);

            if (!type_inspector<table_t>::inspect(L)) {
                throw std::runtime_error("Not a table");
            }
            auto value = table_traits<_key2, reference>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return table_reference<_table<false>, _key2>(value, _table<false>(*this), k);
        }

        template<typename _type>
        constexpr table_reference<_table_type, _key>& operator=(_type other) {
            origin.template set<_key, _type>(at, other);
            return *this;
        }

    };

    template<typename _table, typename _key>
    struct type_traits<table_reference<_table, _key>> : public reference_traits<anonymous_t> {

    };

    template<typename _table, typename _key>
    struct stack_traits<table_reference<_table, _key>> : public reference_stack_traits<anonymous_t> {
    };


}

#endif //TLW_TABLE_REFERENCE_HPP
