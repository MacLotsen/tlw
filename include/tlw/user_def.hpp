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

#include <lua.hpp>
#include <tlw/meta_table.hpp>
#include <tlw/detail/user_ctor.hpp>
#include <tlw/detail/user_prop.hpp>
#include <tlw/detail/user_method.hpp>

namespace tlw {

    template<typename ...>
    struct user_method_def {
    };

    template<>
    struct user_method_def<void()> {
        template<typename _user_type>
        using method_type = void (_user_type::*)();
    };

    template<>
    struct user_method_def<void() const> {
        template<typename _user_type>
        using method_type = void (_user_type::*)() const;
    };

    template<typename _r>
    struct user_method_def<_r()> {
        template<typename _user_type>
        using method_type = _r (_user_type::*)();
    };

    template<typename _r>
    struct user_method_def<_r() const> {
        template<typename _user_type>
        using method_type = _r (_user_type::*)() const;
    };

    template<typename _r, typename ..._args>
    struct user_method_def<_r(_args...)> {
        template<typename _user_type>
        using method_type = _r (_user_type::*)(_args...);
    };

    template<typename _r, typename ..._args>
    struct user_method_def<_r(_args...) const> {
        template<typename _user_type>
        using method_type = _r (_user_type::*)(_args...) const;
    };

    template<typename ..._args>
    struct user_method_def<void(_args...)> {
        template<typename _user_type>
        using method_type = void (_user_type::*)(_args...);
    };

    template<typename ..._args>
    struct user_method_def<void(_args...) const> {
        template<typename _user_type>
        using method_type = void (_user_type::*)(_args...) const;
    };

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
            luaL_error(L, "No suitable constructor found with %d arguments", top);
            return 0;
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
        using ror_mt = meta_table<const _user_type &>;
        using r_mt = meta_table<_user_type &>;

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

            length = strlen(name) + 2;
            char *r_name = new char[length];
            strcpy(r_name, name);
            r_name[length - 2] = '&';
            r_name[length - 1] = '\0';
            r_mt::name = r_name;

            length = strlen(name) + 8;
            char *constp_name = new char[length];
            strcpy(constp_name, "const ");
            strcat(constp_name, name);
            constp_name[length - 2] = '*';
            constp_name[length - 1] = '\0';
            rop_mt::name = constp_name;

            length = strlen(name) + 8;
            char *constr_name = new char[length];
            strcpy(constr_name, "const ");
            strcat(constr_name, name);
            constr_name[length - 2] = '&';
            constr_name[length - 1] = '\0';
            ror_mt::name = constr_name;
        }

        template<typename ..._args>
        _builder_type &ctor() {
            using _ct = user_ctor<_user_type *, _args...>;
            if (mt::constructors.find(sizeof...(_args)) == mt::constructors.end()) {
                mt::constructors[sizeof...(_args)] = type_safe_function_overloads();
            }
            mt::constructors[sizeof...(_args)].push_back(type_safe_function(_ct::check_args, _ct::ctor));
            // a const primitive doesn't need a ctor
            return *this;
        }

        _builder_type &dtor() {
            p_mt::dtor = [](lua_State *L) -> int {
                delete stack_traits<_user_type *>::get(L, -1);
                lua_pop(L, 1);
                return 0;
            };
            rop_mt::dtor = [](lua_State *L) -> int {
                delete stack_traits<const _user_type *>::get(L, -1);
                lua_pop(L, 1);
                return 0;
            };
            return *this;
        }

        template<typename _prop_type>
        _builder_type &prop(const char *name, property_type<_prop_type> property) {
            user_prop<_user_type, _prop_type>::properties[name] = property;
            user_prop<const _user_type, _prop_type>::properties[name] = property;
            user_prop<_user_type *, _prop_type>::properties[name] = property;
            user_prop<const _user_type *, _prop_type>::properties[name] = property;

            mt::getters[name] = user_prop<_user_type, _prop_type>::get;
            ro_mt::getters[name] = user_prop<const _user_type, _prop_type>::get;
            p_mt::getters[name] = user_prop<_user_type *, _prop_type>::get;
            rop_mt::getters[name] = user_prop<const _user_type *, _prop_type>::get;

            if constexpr (cpp_type<_prop_type>::is_const) {
                mt::setters[name] = user_prop<_user_type, _prop_type>::invalid_set;
                ro_mt::setters[name] = user_prop<const _user_type, _prop_type>::invalid_set;
                p_mt::setters[name] = user_prop<_user_type *, _prop_type>::invalid_set;
                rop_mt::setters[name] = user_prop<const _user_type *, _prop_type>::invalid_set;
            } else {
                mt::setters[name] = user_prop<_user_type, _prop_type>::set;
                ro_mt::setters[name] = user_prop<const _user_type, _prop_type>::invalid_set;
                p_mt::setters[name] = user_prop<_user_type *, _prop_type>::set;
                rop_mt::setters[name] = user_prop<const _user_type *, _prop_type>::invalid_set;
            }

            return *this;
        }

//        template<typename _method_type>
//        static inline void register_method(const char *name, _method_type method) {
//            using um = user_method<_method_type>;
//            using mtype = method_type<_method_type>;
//            if (um::methods.find(name) != user_method<_method_type>::methods.end()) {
//                um::methods[name] = arg_counted_function_overloads();
//            }
//            if (um::methods[name].find(mtype::arg_count) != um::methods[name].end()) {
//                um::methods[name][mtype::arg_count] = type_safe_function_overloads();
//            }
//            um::methods[name][mtype::arg_count].push_back(std::tuple{user_method<_method_type>::check_args, method});
//        }

        template<typename _ret_args_type>
        _builder_type &method(const char *name, typename user_method_def<_ret_args_type>::template method_type<_user_type> _method) {
            return method<decltype(_method)>(name, _method);
        }

        template<typename _method_type>
        _builder_type &method(const char *name, _method_type method) {
//            register_method(name, method);
            user_method<_user_type, _method_type>::methods[name] = method;
            user_method<const _user_type, _method_type>::methods[name] = method;
            user_method<_user_type *, _method_type>::methods[name] = method;
            user_method<const _user_type *, _method_type>::methods[name] = method;
            mt::methods[name] = user_method<_user_type, _method_type>::provide;
            p_mt::methods[name] = user_method<_user_type *, _method_type>::provide;
            if (user_method<_user_type, _method_type>::read_only) {
                ro_mt::methods[name] = user_method<const _user_type, _method_type>::provide;
                rop_mt::methods[name] = user_method<const _user_type *, _method_type>::provide;
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
