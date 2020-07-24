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

#ifndef TLW_USER_CTOR_HPP
#define TLW_USER_CTOR_HPP

#include <tlw/stack.hpp>
#include <tlw/detail/util.hpp>
#include <tlw/detail/stack_traits.hpp>

namespace tlw {

    template<class _user_type, typename ..._args>
    struct user_ctor {
        static constexpr bool check_args(lua_State *L) {
            return check_args(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr bool check_args(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static constexpr int ctor(lua_State *L) {
            if constexpr (sizeof...(_args) != 0) {
                auto params = stack(state(L)).grab<_args...>();
                auto seq = gen_seq<sizeof...(_args)>();
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    stack_traits<_user_type>::push(L, create_pointer(params, seq));
                } else {
                    stack_traits<_user_type>::push(L, create_value(params, seq));
                }
            } else {
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    stack_traits<_user_type>::push(L,
                                                   new typename cpp_type<_user_type>::value_type());
                } else {
                    stack_traits<_user_type>::push(L, _user_type());
                }
            }
            return 1;
        }

        template<int ...Ns>
        static constexpr _user_type create_pointer(std::tuple<_args...> t, seq<Ns...>) {
            return new typename cpp_type<_user_type>::value_type(std::get<Ns>(t)...);
        }

        template<int ...Ns>
        static constexpr _user_type create_value(std::tuple<_args...> t, seq<Ns...>) {
            return typename cpp_type<_user_type>::value_type (std::get<Ns>(t)...);
        }
    };

}

#endif //TLW_USER_CTOR_HPP
