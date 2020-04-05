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

    template<typename mt>
    struct __explicit_ctor {

        static int create(lua_State *L) {
            int top = lua_gettop(L);
            if (mt::constructors.empty()) {
                luaL_error(L, "No constructor defined for %s", mt::name);
            }
            if (mt::constructors.find(top) == mt::constructors.end()) {
                luaL_error(L, "No constructor with %d arguments", top);
            }
            auto cset = mt::constructors[top];
            for (auto kv : cset) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
        }

    };

    template<typename _user_type>
    struct __index {
        using mt = meta_table<_user_type>;

        static int get(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.get<const char *>(2);
            auto f = mt::getters[prop];
            if (!f) {
                f = mt::methods[prop];
                if (!f) {
                    luaL_error(L, "Error: no such field '%s'", prop);
                }
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
        using ro_mt = meta_table<const _user_type>;
        using p_mt = meta_table<_user_type *>;
        using rop_mt = meta_table<const _user_type *>;

        template<typename _property_type>
        using property_type = _property_type _user_type::*;

        explicit meta_table_builder(const char *name) noexcept {
            mt::name = name;

            size_t length = strlen(name) + 7;
            char *const_name = new char[length];
            strcpy(const_name, "const ");
            strcat(const_name, name);
            ro_mt::name = const_name;

            length = strlen(name) + 2;
            char *p_name = new char[length];
            strcpy(p_name, name);
            p_name[length - 2] = '*';
            p_name[length - 1] = '\0';
            p_mt::name = p_name;

            length = strlen(name) + 8;
            char *constp_name = new char[length];
            strcpy(constp_name, "const ");
            strcat(constp_name, name);
            constp_name[length - 2] = '*';
            constp_name[length - 1] = '\0';
            rop_mt::name = constp_name;
        }

        template<typename ..._args>
        _builder_type &ctor() {
            using _ct = ctor_traits<_user_type *, _args...>;
            if (mt::constructors.find(sizeof...(_args)) == mt::constructors.end()) {
                mt::constructors[sizeof...(_args)] = ctor_set();
            }
            mt::constructors[sizeof...(_args)].push_back(type_safe_ctor(_ct::check_args, _ct::ctor));
            // a const type doesn't need a ctor
            return *this;
        }

        _builder_type &dtor() {
            p_mt::dtor = [](lua_State *L) -> int {
                delete stack_traits<_user_type *>::peek(L, -1);
                lua_pop(L, 1);
                return 0;
            };
            rop_mt::dtor = [](lua_State *L) -> int {
                delete stack_traits<const _user_type *>::peek(L, -1);
                lua_pop(L, 1);
                return 0;
            };
            return *this;
        }

        template<typename _prop_type>
        _builder_type &prop(const char *name, property_type<_prop_type> property) {
            property_traits<_user_type, _prop_type>::properties[name] = property;
            property_traits<const _user_type, _prop_type>::properties[name] = property;
            property_traits<_user_type *, _prop_type>::properties[name] = property;
            property_traits<const _user_type *, _prop_type>::properties[name] = property;

            mt::getters[name] = property_traits<_user_type, _prop_type>::get;
            ro_mt::getters[name] = property_traits<const _user_type, _prop_type>::get;
            p_mt::getters[name] = property_traits<_user_type *, _prop_type>::get;
            rop_mt::getters[name] = property_traits<const _user_type *, _prop_type>::get;

            if constexpr (const_type<_prop_type>::valid) {
                mt::setters[name] = property_traits<_user_type, _prop_type>::invalid_set;
                ro_mt::setters[name] = property_traits<const _user_type, _prop_type>::invalid_set;
                p_mt::setters[name] = property_traits<_user_type *, _prop_type>::invalid_set;
                rop_mt::setters[name] = property_traits<const _user_type *, _prop_type>::invalid_set;
            } else {
                mt::setters[name] = property_traits<_user_type, _prop_type>::set;
                ro_mt::setters[name] = property_traits<const _user_type, _prop_type>::invalid_set;
                p_mt::setters[name] = property_traits<_user_type *, _prop_type>::set;
                rop_mt::setters[name] = property_traits<const _user_type *, _prop_type>::invalid_set;
            }

            return *this;
        }

        template<typename _method_type>
        _builder_type &method(const char *name, _method_type method) {
            method_traits<_method_type>::methods[name] = method;
            p_mt::methods[name] = method_traits<_method_type>::provide;
            if (method_traits<_method_type>::read_only) {
                rop_mt::methods[name] = method_traits<_method_type>::provide;
            }
            return *this;
        }

        void finish() {
            meta_table_registry<_user_type>::name = mt::name;
            meta_table_registry<const _user_type>::name = ro_mt::name;
            meta_table_registry<_user_type *>::name = p_mt::name;
            meta_table_registry<const _user_type *>::name = rop_mt::name;
            // The const user definition will be exposed by the normal user definition
            meta_table_registry<_user_type>::expose = _expose;
        }

        static void _expose(state L) {
            _expose < mt > (L);
            _expose < ro_mt > (L);
            _expose < p_mt > (L);
            _expose < rop_mt > (L);
        }

        template<typename _mt>
        static void _expose(state L) {
            stack s(L);
            if (!_mt::constructors.empty()) {
                lua_register(L, _mt::name, __explicit_ctor<_mt>::create);
            }

            if (!luaL_newmetatable(L, _mt::name)) {
                printf("Warning: meta table already registered\n");
            }
            int mt_ref = lua_gettop(L);

            s.push(_mt::name);
            lua_setfield(L, mt_ref, "__name");

            if (_mt::dtor) {
                s.push(_mt::dtor);
                lua_setfield(L, mt_ref, "__gc");
            }

            if (!_mt::getters.empty() || !_mt::methods.empty()) {
                s.push(__index<typename _mt::user_type>::get);
                lua_setfield(L, mt_ref, "__index");
            }

            if (!_mt::setters.empty()) {
                s.push(__index<typename _mt::user_type>::set);
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
