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

#ifndef TLW_USER_DEF_HPP
#define TLW_USER_DEF_HPP

#include <tlw/meta_table.hpp>

namespace tlw {

    template<typename ...>
    struct lua_binding {
        virtual cfunction_t::type operator()() {
            return nullptr;
        };
    };

    template<class _user_type, typename ..._args>
    struct lua_ctor_binder_t : public lua_binding<_user_type, _args...> {
        cfunction_t::type operator()() override {
            return [](lua_State *L) -> int {
                if (sizeof...(_args)) {
                    auto params = stack(L).grab<_args...>();
                    auto seq = gen_seq<sizeof...(_args)>();
                    if constexpr (pointer_type<_user_type>::valid) {
                        type_traits<user_data_t<_user_type>>::push(L, create_pointer(params, seq));
                    } else {
                        type_traits<user_data_t<_user_type>>::push(L, create_value(params, seq));
                    }
                } else {
                    if constexpr (pointer_type<_user_type>::valid) {
                        type_traits<user_data_t<_user_type>>::push(L,
                                                                   new typename pointer_type<_user_type>::value_type());
                    } else {
                        type_traits<user_data_t<_user_type>>::push(L, _user_type());
                    }
                }
                return 1;
            };
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

    // TODO mk_class(name, bool traditional)
    // TODO mk_struct (different ctor)
    // TODO mk_function
    // TODO mk_method
    // TODO mk_property

    template<class T>
    constexpr void define(const char *name) {
        if (meta_table_t<T>::name) {
            printf("Warning: meta table %s already defined\n", meta_table_t<T>::name);
            if (strcmp(meta_table_t<T>::name, name) != 0) {
                printf("Warning: changing meta table name from %s to %s\n", meta_table_t<T>::name, name);
            }
        }
        meta_table_t<T>::name = name;
    }

    template<class _user_type, typename ..._args>
    constexpr void define_ctor() {
        if (!meta_table_t<_user_type>::name) {
            printf("Warning: defining constructor while user type has no name\n");
        } else if (meta_table_t<_user_type>::ctor) {
            printf("Warning: redefining constructor for %s\n", meta_table_t<_user_type>::name);
        }
        meta_table_t<_user_type>::ctor = lua_ctor_binder_t<_user_type, _args...>()();
    }
}

#endif //TLW_USER_DEF_HPP
