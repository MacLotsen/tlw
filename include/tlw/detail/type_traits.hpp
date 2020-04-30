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

#ifndef TLW_TYPE_TRAITS_HPP
#define TLW_TYPE_TRAITS_HPP

#include <tlw/detail/util.hpp>
#include <tlw/type.hpp>
#include <tlw/reference.hpp>

namespace tlw {

    template<typename _lua_type_set>
    struct type_inspector {
        using type = _lua_type_set;

        static constexpr bool inspect(lua_State *L) {
            return inspect(L, -1);
        }

        static constexpr bool inspect(lua_State *L, int idx) {
            return lua_type(L, idx) == type::value;
        }
    };

    template<typename _lua_type_set, class...>
    struct type_traits {
        static constexpr typename _lua_type_set::type get(lua_State *L, int idx) {
            throw std::runtime_error("unsupported primitive");
        }

        static constexpr void push(lua_State *L, typename _lua_type_set::type value) {
            throw std::runtime_error("unsupported primitive");
        }
    };

    template<typename _lua_type_set>
    struct reference_traits {
        static constexpr typename _lua_type_set::type get(lua_State *L, int idx) {
            lua_pushvalue(L, idx);
            return reference(state(L));
        }

        static constexpr void push(lua_State *L, typename _lua_type_set::type& value) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, value.r_idx);
        }
    };

    template<typename _lua_type_set, class ..._args>
    struct ordered_type_traits : public type_traits<_lua_type_set, _args...> {
        static constexpr typename _lua_type_set::type get(lua_State *L) {
            return type_traits<_lua_type_set, _args...>::get(L, -1);
        }
    };

    template<>
    struct type_traits<none_t> {
        static constexpr typename none_t::type get(lua_State *, int) {}

        static constexpr void push(lua_State *) {}
    };

    template<>
    struct type_traits<nil_t> {
        static constexpr typename nil_t::type get(lua_State *L, int idx) {
            return nullptr;
        }

        static void push(lua_State *L, typename nil_t::type) {
            lua_pushnil(L);
        }
    };

    template<>
    struct type_traits<bool_t> {
        static typename bool_t::type get(lua_State *L, int idx) {
            return lua_toboolean(L, idx) != 0;
        }

        static void push(lua_State *L, typename bool_t::type value) {
            lua_pushboolean(L, value);
        }
    };

    template<>
    struct type_traits<number_t> {
        static typename number_t::type get(lua_State *L, int idx) {
            return lua_tonumber(L, idx);
        }

        static void push(lua_State *L, typename number_t::type value) {
            lua_pushnumber(L, value);
        }
    };

    template<>
    struct type_traits<integer_t> {
        static typename number_t::type get(lua_State *L, int idx) {
            return lua_tointeger(L, idx);
        }

        static void push(lua_State *L, typename number_t::type value) {
            lua_pushinteger(L, value);
        }
    };

    template<>
    struct type_traits<string_t> {
        static typename string_t::type get(lua_State *L, int idx) {
            return lua_tostring(L, idx);
        }

        static void push(lua_State *L, typename string_t::type value) {
            lua_pushstring(L, value);
        }
    };

    template<>
    struct type_traits<table_t> : public reference_traits<table_t> {
        using reference_traits<table_t>::get;
        using reference_traits<table_t>::push;
    };

    template<>
    struct type_traits<function_t> : public reference_traits<function_t> {
        using reference_traits<function_t>::get;
        using reference_traits<function_t>::push;
    };

    template<>
    struct type_traits<cfunction_t> : public reference_traits<function_t> {
        using reference_traits<function_t>::get;

        static void push(lua_State *L, typename cfunction_t::type value) {
            lua_pushcfunction(L, value);
        }
    };

    template<class _user_type>
    struct type_traits<user_data_t<_user_type>> {
        static constexpr _user_type& get(lua_State *L, int idx) {
            auto user_data = (_user_type *) lua_touserdata(L, idx);
            return *user_data;
        }

        static constexpr void push(lua_State *L, _user_type &value) {
            auto user_data = (_user_type *) lua_newuserdata(L, sizeof(_user_type));
            if constexpr (cpp_type<_user_type>::is_rvalue) {
                *user_data = std::move(value);
            } else {
                *user_data = value;
            }
        }
    };

    template<class _user_type>
    struct type_traits<user_data_t<const _user_type>> {
        static constexpr const _user_type& get(lua_State *L, int idx) {
            auto user_data = (const _user_type *) lua_touserdata(L, idx);
            return *user_data;
        }

        static constexpr void push(lua_State *L, const _user_type &value) {
            auto user_data = (_user_type *) lua_newuserdata(L, sizeof(_user_type));
            *user_data = value;
        }
    };

    template<class _user_type>
    struct type_traits<user_data_t<_user_type&>> {
        static constexpr _user_type& get(lua_State *L, int idx) {
            auto user_data = (_user_type **) lua_touserdata(L, idx);
            return **user_data;
        }

        static constexpr void push(lua_State *L, _user_type &value) {
            auto user_data = (_user_type **) lua_newuserdata(L, sizeof(_user_type**));
            *user_data = &value;
        }
    };

    template<class _user_type>
    struct type_traits<user_data_t<_user_type*>> {
        static constexpr _user_type* get(lua_State *L, int idx) {
            auto user_data = (_user_type **) lua_touserdata(L, idx);
            return *user_data;
        }

        static constexpr void push(lua_State *L, _user_type* value) {
            auto user_data = (_user_type **) lua_newuserdata(L, sizeof(_user_type **));
            *user_data = value;
        }
    };

    template<class _user_type>
    struct type_traits<user_data_t<const _user_type*>> {
        static constexpr const _user_type* get(lua_State *L, int idx) {
            auto user_data = (const _user_type **) lua_touserdata(L, idx);
            return *user_data;
        }

        static constexpr void push(lua_State *L, const _user_type* value) {
            auto user_data = (const _user_type **) lua_newuserdata(L, sizeof(const _user_type **));
            *user_data = value;
        }
    };

//    template<class _light_user_type>
//    struct type_traits<light_user_data_t<_light_user_type>> {
//        static constexpr _light_user_type& get(lua_State *L, int idx) {
//            auto user_data = (_light_user_type *) lua_touserdata(L, idx);
//            return *user_data;
//        }
//
//        static constexpr void push(lua_State *L, _light_user_type& value) {
//            if constexpr (cpp_type<_light_user_type>::is_const) {
//                luaL_error(L, "Cannot push a const value as light user data");
//            } else {
//                lua_pushlightuserdata(L, &value);
//            }
//        }
//    };

//    template<class _light_user_type>
//    struct type_traits<light_user_data_t<_light_user_type&>> {
//        static constexpr _light_user_type& get(lua_State *L, int idx) {
//            auto user_data = (_light_user_type *) lua_touserdata(L, idx);
//            return *user_data;
//        }
//
//        static constexpr void push(lua_State *L, _light_user_type& value) {
//            if constexpr (cpp_type<_light_user_type>::is_const) {
//                luaL_error(L, "Cannot push a const value as light user data");
//            } else {
//                lua_pushlightuserdata(L, &value);
//            }
//        }
//    };

    template<class _light_user_type>
    struct type_traits<light_user_data_t<_light_user_type *>> {
        static constexpr _light_user_type *get(lua_State *L, int idx) {
            return (_light_user_type *) lua_touserdata(L, idx);
        }

        static constexpr void push(lua_State *L, _light_user_type *value) {
            if constexpr (cpp_type<_light_user_type>::is_const) {
                luaL_error(L, "Cannot push a const value as light user data");
            } else {
                lua_pushlightuserdata(L, value);
            }
        }
    };
}

#endif //TLW_TYPE_TRAITS_HPP
