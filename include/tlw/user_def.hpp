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
#include <tlw/operators.hpp>
#include <cstring>

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

        static constexpr int create(lua_State *L) {
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

    template<typename mt>
    struct __explicit_method {

        static constexpr int invoke(lua_State *L) {
            auto ud = stack_traits<typename mt::user_type>::get(L, 1);
            auto prop = stack_traits<const char *>::get(L, 2);

            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<typename mt::user_type>(
                        lua_upvalueindex(1));//stack_traits<typename mt::user_type>::get(L, lua_upvalueindex(1));
                auto prop = s.get<const char *>(
                        lua_upvalueindex(2));//stack_traits<const char *>::get(L, lua_upvalueindex(2));

                int top = lua_gettop(L);
                if (mt::methods.empty()) {
                    luaL_error(L, "No methods defined for %s", mt::name);
                }

                if (mt::methods.find(prop) == mt::methods.end()) {
                    luaL_error(L, "No method %s defined for %s", prop, mt::name);
                }

                if (mt::methods[prop].find(top) == mt::methods[prop].end()) {
                    luaL_error(L, "No method %s.%s with %d arguments", mt::name, prop, top);
                }

                auto mset = mt::methods[prop][top];
                for (auto kv : mset) {
                    if (std::get<0>(kv)(L)) {
                        auto returns = std::get<1>(kv)(s, ud, prop);
                        if (returns < 0) {
                            luaL_error(L, "Method %s.%s is readonly", mt::name, prop);
                            return 0;
                        }
                        return returns;
                    }
                }
                luaL_error(L, "No suitable method found with %d arguments", top);
                return 0;
            }, 2);

            return 1;
        }

    };

    template<typename mt>
    struct __explicit_operator {

        static constexpr int add(lua_State *L) {
            for (auto kv : mt::add_operator) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
            luaL_error(L, "ERROR: Cannot sum up");
            return 0;
        }

        static constexpr int sub(lua_State *L) {
            for (auto kv : mt::sub_operator) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
            luaL_error(L, "ERROR: Cannot sum up");
            return 0;
        }

        static constexpr int mul(lua_State *L) {
            for (auto kv : mt::mul_operator) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
            luaL_error(L, "ERROR: Cannot sum up");
            return 0;
        }

        static constexpr int div(lua_State *L) {
            for (auto kv : mt::div_operator) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
            luaL_error(L, "ERROR: Cannot sum up");
            return 0;
        }

        static constexpr int mod(lua_State *L) {
            for (auto kv : mt::mod_operator) {
                if (std::get<0>(kv)(L)) {
                    return std::get<1>(kv)(L);
                }
            }
            luaL_error(L, "ERROR: Cannot sum up");
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
                f = __explicit_method<mt>::invoke;
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

    template<class _user_type>
    struct meta_table_builder {
        using _builder_type = meta_table_builder<_user_type>;
        using mt = meta_table<_user_type>;
        using ro_mt = meta_table<const _user_type>;
        using p_mt = meta_table<_user_type *>;
        using rop_mt = meta_table<const _user_type *>;

        template<typename _property_type>
        using property_type = _property_type _user_type::*;

        constexpr explicit meta_table_builder(const char *name) noexcept {
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
        constexpr _builder_type &ctor() {
            using _ct = user_ctor<_user_type, _args...>;
            using _pct = user_ctor<_user_type *, _args...>;
            if (mt::constructors.find(sizeof...(_args)) == mt::constructors.end()) {
                mt::constructors[sizeof...(_args)] = type_safe_function_overloads();
            }
            mt::constructors[sizeof...(_args)].push_back(type_safe_function(_ct::check_args, _ct::ctor));
            if (p_mt::constructors.find(sizeof...(_args)) == p_mt::constructors.end()) {
                p_mt::constructors[sizeof...(_args)] = type_safe_function_overloads();
            }
            p_mt::constructors[sizeof...(_args)].push_back(type_safe_function(_pct::check_args, _pct::ctor));
            return *this;
        }

        template<typename _prop_type>
        constexpr _builder_type &prop(const char *name, property_type<_prop_type> property) {
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

        template<typename mt, typename _method_type>
        static constexpr void _register_method(const char *prop, _method_type method) {
            using um = user_method<typename mt::user_type, _method_type>;
            using mtype = method_type<_method_type>;
            if (mt::methods.find(prop) == mt::methods.end()) {
                mt::methods[prop] = arg_counted_overloads<type_safe_method_overloads<typename mt::user_type>>();
            }
            if (mt::methods[prop].find(mtype::arg_count) == mt::methods[prop].end()) {
                mt::methods[prop][mtype::arg_count] = type_safe_method_overloads<typename mt::user_type>();
            }

            mt::methods[prop][mtype::arg_count].push_back(
                    type_safe_method<typename mt::user_type>(&um::test, &um::provide));
        }

        template<typename _ret_args_type>
        constexpr _builder_type &
        method(const char *name, typename user_method_def<_ret_args_type>::template method_type<_user_type> _method) {
            return method<decltype(_method)>(name, _method);
        }

        template<typename _method_type>
        constexpr _builder_type &method(const char *name, _method_type method) {
            user_method<_user_type, _method_type>::methods[name] = method;
            user_method<const _user_type, _method_type>::methods[name] = method;
            user_method<_user_type *, _method_type>::methods[name] = method;
            user_method<const _user_type *, _method_type>::methods[name] = method;
            _register_method<mt>(name, method);
            _register_method<ro_mt>(name, method);
            _register_method<p_mt>(name, method);
            _register_method<rop_mt>(name, method);
            return *this;
        }

        constexpr _builder_type &unm() {
            mt::operators["__unm"] = __operator<_user_type>::unm;
            ro_mt::operators["__unm"] = __operator<const _user_type>::unm;
            p_mt::operators["__unm"] = __operator<_user_type*>::unm;
            rop_mt::operators["__unm"] = __operator<const _user_type*>::unm;
            return *this;
        }

        template<typename _method_type>
        constexpr _builder_type &len(_method_type method) {
            using _r = typename method_type<_method_type>::return_type;
            __len<_user_type, _r>::size_method = method;
            __len<const _user_type, _r>::size_method = method;
            __len<_user_type*, _r>::size_method = method;
            __len<const _user_type*, _r>::size_method = method;
            mt::operators["__len"] = __len<_user_type, _r>::len;
            ro_mt::operators["__len"] = __len<const _user_type, _r>::len;
            p_mt::operators["__len"] = __len<_user_type*, _r>::len;
            rop_mt::operators["__len"] = __len<const _user_type*, _r>::len;
            return *this;
        }

        constexpr _builder_type &tostring(typename __tostring<_user_type>::tostring_method_t method) {
            __tostring<_user_type>::tostring_method = method;
            __tostring<const _user_type>::tostring_method = method;
            __tostring<_user_type*>::tostring_method = method;
            __tostring<const _user_type*>::tostring_method = method;
            mt::operators["__tostring"] = __tostring<_user_type>::tostring;
            ro_mt::operators["__tostring"] = __tostring<const _user_type>::tostring;
            p_mt::operators["__tostring"] = __tostring<_user_type*>::tostring;
            rop_mt::operators["__tostring"] = __tostring<const _user_type*>::tostring;
        }

        template<typename _other>
        constexpr _builder_type &add() {
            mt::add_operator.push_back(type_safe_function{__add<_user_type, _other>::test, __add<_user_type, _other>::add});
            ro_mt::add_operator.push_back(type_safe_function{__add<const _user_type, _other>::test, __add<const _user_type, _other>::add});
            p_mt::add_operator.push_back(type_safe_function{__add<_user_type*, _other>::test, __add<_user_type*, _other>::add});
            rop_mt::add_operator.push_back(type_safe_function{__add<const _user_type*, _other>::test, __add<const _user_type*, _other>::add});
            return *this;
        }

        template<typename _other, typename _other2, typename ..._others>
        constexpr _builder_type &add() {
            add<_other>(); add<_other2>();
            (..., add<_others>());
            return *this;
        }

        template<typename _other>
        constexpr _builder_type &sub() {
            mt::sub_operator.push_back(type_safe_function{__sub<_user_type, _other>::test, __sub<_user_type, _other>::sub});
            ro_mt::sub_operator.push_back(type_safe_function{__sub<const _user_type, _other>::test, __sub<const _user_type, _other>::sub});
            p_mt::sub_operator.push_back(type_safe_function{__sub<_user_type*, _other>::test, __sub<_user_type*, _other>::sub});
            rop_mt::sub_operator.push_back(type_safe_function{__sub<const _user_type*, _other>::test, __sub<const _user_type*, _other>::sub});
            return *this;
        }

        template<typename _other, typename _other2, typename ..._others>
        constexpr _builder_type &sub() {
            sub<_other>(); sub<_other2>();
            (..., sub<_others>());
            return *this;
        }

        template<typename _other>
        constexpr _builder_type &mul() {
            mt::mul_operator.push_back(type_safe_function{__mul<_user_type, _other>::test, __mul<_user_type, _other>::mul});
            ro_mt::mul_operator.push_back(type_safe_function{__mul<const _user_type, _other>::test, __mul<const _user_type, _other>::mul});
            p_mt::mul_operator.push_back(type_safe_function{__mul<_user_type*, _other>::test, __mul<_user_type*, _other>::mul});
            rop_mt::mul_operator.push_back(type_safe_function{__mul<const _user_type*, _other>::test, __mul<const _user_type*, _other>::mul});
            return *this;
        }

        template<typename _other, typename _other2, typename ..._others>
        constexpr _builder_type &mul() {
            mul<_other>(); mul<_other2>();
            (..., mul<_others>());
            return *this;
        }

        template<typename _other>
        constexpr _builder_type &div() {
            mt::div_operator.push_back(type_safe_function{__div<_user_type, _other>::test, __div<_user_type, _other>::div});
            ro_mt::div_operator.push_back(type_safe_function{__div<const _user_type, _other>::test, __div<const _user_type, _other>::div});
            p_mt::div_operator.push_back(type_safe_function{__div<_user_type*, _other>::test, __div<_user_type*, _other>::div});
            rop_mt::div_operator.push_back(type_safe_function{__div<const _user_type*, _other>::test, __div<const _user_type*, _other>::div});
            return *this;
        }

        template<typename _other, typename _other2, typename ..._others>
        constexpr _builder_type &div() {
            div<_other>(); div<_other2>();
            (..., div<_others>());
            return *this;
        }

        template<typename _other>
        constexpr _builder_type &mod() {
            mt::mod_operator.push_back(type_safe_function{__mod<_user_type, _other>::test, __mod<_user_type, _other>::mod});
            ro_mt::mod_operator.push_back(type_safe_function{__mod<const _user_type, _other>::test, __mod<const _user_type, _other>::mod});
            p_mt::mod_operator.push_back(type_safe_function{__mod<_user_type*, _other>::test, __mod<_user_type*, _other>::mod});
            rop_mt::mod_operator.push_back(type_safe_function{__mod<const _user_type*, _other>::test, __mod<const _user_type*, _other>::mod});
            return *this;
        }

        template<typename _other, typename _other2, typename ..._others>
        constexpr _builder_type &mod() {
            mod<_other>(); mod<_other2>();
            (..., mod<_others>());
            return *this;
        }

        template<typename _method_type>
        constexpr _builder_type &tostring(_method_type method) {
            __tostring<_user_type>::tostring_method = method;
            __tostring<const _user_type>::tostring_method = method;
            __tostring<_user_type*>::tostring_method = method;
            __tostring<const _user_type*>::tostring_method = method;
            mt::operators["__tostring"] = __tostring<_user_type>::tostring;
            ro_mt::operators["__tostring"] = __tostring<const _user_type>::tostring;
            p_mt::operators["__tostring"] = __tostring<_user_type*>::tostring;
            rop_mt::operators["__tostring"] = __tostring<const _user_type*>::tostring;
        }

        lib_load_t finish() {
            meta_table_registry<_user_type>::name = mt::name;
            meta_table_registry<const _user_type>::name = ro_mt::name;
            meta_table_registry<_user_type *>::name = p_mt::name;
            meta_table_registry<const _user_type *>::name = rop_mt::name;
            return meta_table_registry<_user_type>::expose = _expose;
        }

        static void _expose(state L) {
            if (!mt::constructors.empty()) {
                lua_createtable(L, 0, 0);
                lua_createtable(L, 0, 3);
                lua_createtable(L, 0, 1);
                lua_pushcfunction(L, __explicit_ctor<p_mt>::create);
                lua_setfield(L, -2, "new");
                lua_setfield(L, -2, "__index");
                lua_pushliteral(L, "private");
                lua_setfield(L, -2, "__metatable");
                lua_pushcfunction(L, [](lua_State *L)->int {
                    // remove the table
                    lua_remove(L, 1);
                    return __explicit_ctor<mt>::create(L);
                });
                lua_setfield(L, -2, "__call");
                lua_setmetatable(L, -2);
                lua_setglobal(L, mt::name);
            }

            _expose < mt > (L);
            _expose < ro_mt > (L);
            _expose < p_mt > (L);
            _expose < rop_mt > (L);
        }

        template<typename _mt>
        static void _expose(state L) {
            stack s(L);

            if (!luaL_newmetatable(L, _mt::name)) {
                printf("Warning: meta table already registered\n");
            }
            int mt_ref = lua_gettop(L);

            for (auto kv : _mt::operators) {
                s.push(kv.second);
                lua_setfield(L, mt_ref, std::string(kv.first).c_str());
            }

            if (!_mt::add_operator.empty()) {
                s.push(__explicit_operator<_mt>::add);
                lua_setfield(L, mt_ref, "__add");
            }

            if (!_mt::sub_operator.empty()) {
                s.push(__explicit_operator<_mt>::sub);
                lua_setfield(L, mt_ref, "__sub");
            }

            if (!_mt::mul_operator.empty()) {
                s.push(__explicit_operator<_mt>::mul);
                lua_setfield(L, mt_ref, "__mul");
            }

            if (!_mt::div_operator.empty()) {
                s.push(__explicit_operator<_mt>::div);
                lua_setfield(L, mt_ref, "__div");
            }

            if (!_mt::mod_operator.empty()) {
                s.push(__explicit_operator<_mt>::mod);
                lua_setfield(L, mt_ref, "__mod");
            }

            s.push(_mt::name);
            lua_setfield(L, mt_ref, "__name");

            if (!_mt::getters.empty() || !_mt::methods.empty()) {
                s.push(__index<typename _mt::user_type>::get);
                lua_setfield(L, mt_ref, "__index");
            }

            if (!_mt::setters.empty()) {
                s.push(__index<typename _mt::user_type>::set);
                lua_setfield(L, mt_ref, "__newindex");
            }

            if (_mt::operators.find("__tostring") == _mt::operators.end()) {
                s.push(__operator<typename _mt::user_type>::to_string);
                lua_setfield(L, mt_ref, "__tostring");
            }

            lua_settop(L, mt_ref - 1);
        }
    };

    template<class _user_type>
    constexpr meta_table_builder<_user_type> define(const char *name) {
        if (meta_table_registry<_user_type>::name) {
            printf("Warning: meta table %s already defined\n", meta_table_registry<_user_type>::name);
            if (strcmp(meta_table_registry<_user_type>::name, name) != 0) {
                meta_table_registry<_user_type>::reset();
                printf("Warning: changing meta table name from %s to %s\n", meta_table_registry<_user_type>::name,
                       name);
            }
        }
        return meta_table_builder<_user_type>(name);
    }
}

#endif //TLW_USER_DEF_HPP
