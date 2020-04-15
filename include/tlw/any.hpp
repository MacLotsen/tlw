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

#ifndef TLW_ANY_HPP
#define TLW_ANY_HPP

#include <lua.hpp>
#include <tlw/reference.hpp>
#include <tlw/detail/stack_traits.hpp>

namespace tlw {

    struct any;

    template<>
    struct type_traits<any> : public reference_traits<anonymous_t> {

    };

    template<>
    struct stack_traits<any> : public reference_stack_traits<anonymous_t> {
    };

    struct any : public reference {

        any(const reference &other) : reference(other) {

        }

        any(reference &&other) : reference(std::move(other)) {

        }

        template<typename _type>
        operator _type() {
            using traits = stack_traits<_type>;
            stack_traits<any>::push(L, *this);
            if (!traits::inspect(L, -1)) {
                printf("primitive conversion failed. Actual: %s Given: %s\n", luaL_typename(L, -1), typeid(_type).name());
                throw std::runtime_error("invalid primitive conversion");
            }
            auto val = traits::get(L, -1);
            lua_pop(L, 1);
            return val;
        }

        template<typename _key>
        any operator[](_key k) {
            stack_traits<any>::push(L, *this);

            if (!type_inspector<table_t>::inspect(L)) {
                throw std::runtime_error("Not a table");
            }
            auto value = std::move(table_traits<any, _key>::get(L, lua_gettop(L), k));
            lua_pop(L, 1);
            return value;
        }
    };

}

#endif //TLW_ANY_HPP
