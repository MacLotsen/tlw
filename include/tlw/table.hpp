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
#include <tlw/_table.hpp>
#include <tlw/table_reference.hpp>

namespace tlw {

    template<typename _table_type, typename _key_type>
    struct table_iterator {
        using _value_type = table_reference<_table_type, _key_type>;
        using iter = table_iterator<_table_type, _key_type>;
        using kvp = std::pair<_key_type, _value_type>;

        state L;
        reference table;
        int top;

        table_iterator(tlw::reference &t) {
            L = t.L;
            table = t;
            top = lua_gettop(L);
            stack_traits<tlw::reference>::push(L, t);
            lua_pushnil(L);
            lua_pushnil(L);
            this->operator++();
        }

        table_iterator() {
            L = state::invalid_state();
            table = reference();
            top = -1;
        }

        ~table_iterator() {
            if (top > -1)
                lua_settop(L, top);
        }

        kvp operator*() {
            _key_type key = stack_traits<_key_type>::get(L, top + 2);
            _value_type val = table_reference(reference(L, top + 3), _table<false>(table), key);
            return {key, val};
        }

        iter &operator++() {
            do {
                // Firstly pop the old value
                lua_pop(L, 1);
                if (!lua_next(L, top + 1)) {
                    table = reference();
                    lua_settop(L, top + 2);
                    break;
                }
            } while (!stack_traits<_key_type>::inspect(L, top + 2));

            return *this;
        }

        iter operator++(int) {
            auto copy = *this;
            this->operator++();
            return copy;
        }

        bool operator==(const iter &other) {
            return table == other.table;
        }

        bool operator!=(const iter &other) {
            return table != other.table;
        }
    };

    template<>
    struct _table<true> {
        state L;

        _table(state L) : L(std::move(L)) {}

        template<typename _key, typename _type>
        constexpr _type get(_key k) {
            return table_traits<_key, _type>::get(L, LUA_GLOBALSINDEX, k);
        }

        template<typename _key, typename _type>
        constexpr _type get_or_default(_key k, _type default_value) {
            _type r = table_traits<_key, _type>::get(L, LUA_GLOBALSINDEX, k);
            return r ? r : default_value;
        }

        template<typename ..._rs, typename ..._ks>
        constexpr std::tuple<_rs...> get_all(_ks ...k) {
            return {table_traits<_rs, _ks>::get(L, LUA_GLOBALSINDEX, k)...};
        }

        template<typename _key, typename _type>
        constexpr void set(_key k, _type val) {
            if constexpr (cpp_type<_type>::is_rvalue) {
                table_traits<_key, _type>::set(L, LUA_GLOBALSINDEX, k, std::forward(val));
            } else {
                table_traits<_key, _type>::set(L, LUA_GLOBALSINDEX, k, val);
            }
        }

        template<typename _key>
        constexpr table_reference <_table<true>, _key> operator[](_key k) {
            return table_reference<_table<true>, _key>(get<_key, reference>(k), *this, k);
        }
    };

    template<>
    struct _table<false> : public reference {
        using traits = stack_traits<reference>;
        using iterator = table_iterator<_table<false>, const char *>;

        _table(const reference &other) : reference(other) {

        }

        _table(reference &&other) : reference(std::move(other)) {

        }

        template<typename _key, typename _type>
        constexpr _type get(_key k) {
            traits::push(L, *this);
            auto value = table_traits<_key, _type>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return value;
        }

        template<typename _key, typename _type>
        constexpr _type get_or_default(_key k, _type default_value) {
            traits::push(L, *this);
            _type r = table_traits<_key, _type>::get(L, lua_gettop(L), k);
            lua_pop(L, 1);
            return r ? r : default_value;
        }

        template<typename ..._ks, typename ..._rs>
        constexpr std::tuple<_rs...> get_all(_ks ...k) {
            traits::push(L, *this);
            auto r = std::tuple{table_traits<_ks, _rs>::get(L, lua_gettop(L), k)...};
            lua_pop(L, 1);
            return r;
        }

        template<typename _key, typename _type>
        constexpr void set(_key k, _type val) {
            traits::push(L, *this);
            if constexpr (cpp_type<_type>::is_rvalue) {
                table_traits<_key, _type>::set(L, lua_gettop(L), k, std::forward(val));
            } else {
                table_traits<_key, _type>::set(L, lua_gettop(L), k, val);
            }
            lua_pop(L, 1);
        }

        template<typename _key>
        constexpr table_reference <_table<false>, _key> operator[](_key k) {
            return table_reference<_table<false>, _key>(get<_key, reference>(k), *this, k);
        }

        iterator begin() const {
            return iterator((reference &) *this);
        }

        iterator end() const {
            return iterator();
        }
    };

    template<>
    struct stack_traits<table> : public type_inspector<table_t> {
        using type_inspector<table_t>::inspect;
        static constexpr const bool is_lua_type = true;

        static void push(lua_State *L, reference &value) {
            reference_traits<table_t>::push(L, value);
        }

        static table get(lua_State *L, int idx) {
            return table(reference_traits<table_t>::get(L, idx));
        }
    };

}

#endif //TLW_TABLE_HPP
