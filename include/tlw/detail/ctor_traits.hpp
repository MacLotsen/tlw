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

#ifndef TLW_CTOR_TRAITS_HPP
#define TLW_CTOR_TRAITS_HPP

#include <tlw/stack.hpp>
#include <tlw/detail/util.hpp>
#include <tlw/detail/stack_traits.hpp>

namespace tlw {

    template<class _user_type, typename ..._args>
    struct ctor_traits {
        static bool check_args(lua_State *L) {
            return check_args(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static bool check_args(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static int ctor(lua_State *L) {
            if constexpr (sizeof...(_args)) {
                auto params = stack(state(L)).grab<_args...>();
                auto seq = gen_seq<sizeof...(_args)>();
                if constexpr (pointer_type<_user_type>::valid) {
                    stack_traits<_user_type>::push(L, create_pointer(params, seq));
                } else {
                    stack_traits<_user_type>::push(L, create_value(params, seq));
                }
            } else {
                if constexpr (pointer_type<_user_type>::valid) {
                    stack_traits<_user_type>::push(L,
                                                   new typename pointer_type<_user_type>::value_type());
                } else {
                    stack_traits<_user_type>::push(L, _user_type());
                }
            }
            return 1;
        }

        template<int ...Ns>
        constexpr static _user_type create_pointer(std::tuple<_args...> t, seq<Ns...>) {
            return new typename pointer_type<_user_type>::value_type(std::get<Ns>(t)...);
        }

        template<int ...Ns>
        constexpr static _user_type create_value(std::tuple<_args...> t, seq<Ns...>) {
            return _user_type(std::get<Ns>(t)...);
        }
    };

}

#endif //TLW_CTOR_TRAITS_HPP
