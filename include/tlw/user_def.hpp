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
#include <tlw/detail/ctor_traits.hpp>
#include <tlw/detail/property_traits.hpp>
#include <tlw/detail/method_traits.hpp>

namespace tlw {

    template<typename _user_type>
    struct __index {
        using mt = meta_table<_user_type>;

        static int get(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.get<const char *>(2);
            auto f = mt::getters[prop];
            if (!f) {
                f = mt::methods[prop];
            }
            if (!f) {
                luaL_error(L, "Error: no such field '%s'", prop);
            }
            return f(L);
        }

        static int set(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.get<const char *>(2);
            auto f = mt::setters[prop];
            if (!f) {
                luaL_error(L, "Error: no such property '%s'", prop);
            }
            return f(L);
        }
    };

    // TODO mk_struct (different ctor)
    // TODO mk_function
    // TODO mk_method

    template<class _user_type, bool _traditional>
    struct meta_table_builder {
        using _builder_type = meta_table_builder<_user_type, _traditional>;
        using mt = meta_table<_user_type>;

        template<typename _property_type>
        using property_type = _property_type pointer_type<_user_type>::value_type::*;

        explicit meta_table_builder(const char *name) noexcept {
            mt::name = name;
        }

        template<typename ..._args>
        _builder_type &ctor() {
            mt::ctor = ctor_traits<_user_type, _args...>::ctor;
        }

        _builder_type &dtor() {
            mt::dtor = [](lua_State *L) -> int {
                delete stack_traits<_user_type>::peek(L, -1);
                lua_pop(L, 1);
                return 0;
            };
        }

        template<typename _prop_type>
        _builder_type &prop(const char *name, property_type<_prop_type> property) {
            property_traits<_user_type, _prop_type>::properties[name] = property;
            mt::getters[name] = property_traits<_user_type, _prop_type>::get;
            mt::setters[name] = property_traits<_user_type, _prop_type>::set;
        }

        template<typename _method_type>
        _builder_type &method(const char *name, _method_type method) {
            method_traits<_method_type>::methods[name] = method;
            mt::methods[name] = method_traits<_method_type>::provide;
        }

        void finish() {
            meta_table_registry<_user_type>::name = mt::name;
            meta_table_registry<_user_type>::expose = _expose;
        }

        static void _expose(state L) {
            stack s(L);
            if (mt::ctor) {
                lua_register(L, mt::name, mt::ctor);
            }

            if (luaL_newmetatable(L, mt::name)) {
                printf("Warning: meta table already registered\n");
            }
            int mt_ref = lua_gettop(L);

            if (mt::dtor) {
                s.push(mt::dtor);
                lua_setfield(L, mt_ref, "__gc");
            }

            if (!mt::getters.empty() || !mt::methods.empty()) {
                s.push(__index<_user_type>::get);
                lua_setfield(L, mt_ref, "__index");
            }

            if (!mt::setters.empty()) {
                s.push(__index<_user_type>::set);
                lua_setfield(L, mt_ref, "__newindex");
            }

            lua_settop(L, mt_ref - 1);
        }
    };

    template<class _user_type, bool traditional>
    constexpr meta_table_builder<_user_type, traditional> define(const char *name) {
        if (meta_table_registry<_user_type>::name) {
            printf("Warning: meta table %s already defined\n", meta_table_registry<_user_type>::name);
            if (strcmp(meta_table_registry<_user_type>::name, name) != 0) {
                meta_table_registry<_user_type>::reset();
                printf("Warning: changing meta table name from %s to %s\n", meta_table_registry<_user_type>::name,
                       name);
            }
        }
        return meta_table_builder<_user_type, traditional>(name);
    }

    template<class _user_type>
    constexpr meta_table_builder<_user_type, true> define(const char *name) {
        return define<_user_type, true>(name);
    }
}

#endif //TLW_USER_DEF_HPP
