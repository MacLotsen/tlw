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

#ifndef TLW_USER_METHOD_HPP
#define TLW_USER_METHOD_HPP

namespace tlw {

    template<typename _type>
    _type get_arg(stack &s, int &idx) {
        return s.get<_type>(++idx);
    }



    template<typename ...>
    struct user_method {

    };

    template<typename _user_type, typename _base_type>
    struct user_method<_user_type, void (_base_type::*)() const> {
        using method_t = void (_base_type::*)() const;
        static constexpr const bool read_only = true;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.get<_user_type>(1);
            s.push(&methods[prop]);
            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<_user_type>(lua_upvalueindex(1));
                auto m = s.get<method_t>(lua_upvalueindex(2));
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    (ud->*m)();
                } else {
                    (ud.*m)();
                }
                return 0;
            }, 2);
            return 1;
        }
    };

    template<typename _user_type, typename _base_type>
    struct user_method<_user_type, void (_base_type::*)()> {
        using method_t = void (_base_type::*)();
        static constexpr const bool read_only = false;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            if constexpr (cpp_type<_user_type>::is_const) {
                luaL_error(L, "%s is read only and method %s isn't marked const either.", meta_table<_user_type>::name, prop);
                return 0;
            } else {
                auto ud = s.get<_user_type>(1);
                s.push(&methods[prop]);
                lua_pushcclosure(L, [](lua_State *L) -> int {
                    stack s = stack(state(L));
                    auto ud = s.get<_user_type>(lua_upvalueindex(1));
                    auto m = s.get<method_t>(lua_upvalueindex(2));
                    if constexpr (cpp_type<_user_type>::is_pointer) {
                        (ud->*m)();
                    } else {
                        (ud.*m)();
                    }
                    return 0;
                }, 2);
                return 1;
            }
        }
    };

    /*
     * WITH RETURN
     */

    template<typename _user_type, typename _base_type, typename _r>
    struct user_method<_user_type, _r (_base_type::*)() const> {
        using method_t = _r (_base_type::*)() const;
        static constexpr const bool read_only = true;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.get<_user_type>(1);
            s.push(&methods[prop]);
            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<_user_type>(lua_upvalueindex(1));
                auto m = s.get<method_t>(lua_upvalueindex(2));
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    s.push((ud->*m)());
                } else {
                    s.push((ud.*m)());
                }
                return 1;
            }, 2);
            return 1;
        }
    };

    template<typename _user_type, typename _base_type, typename _r>
    struct user_method<_user_type, _r (_base_type::*)()> {
        using method_t = _r (_base_type::*)();
        static constexpr const bool read_only = false;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            if constexpr (cpp_type<_user_type>::is_const) {
                luaL_error(L, "%s is read only and method %s isn't marked const either.", meta_table<_user_type>::name, prop);
                return 0;
            } else {
                auto ud = s.get<_user_type>(1);
                s.push(&methods[prop]);
                lua_pushcclosure(L, [](lua_State *L) -> int {
                    stack s = stack(state(L));
                    auto ud = s.get<_user_type>(lua_upvalueindex(1));
                    auto m = s.get<method_t>(lua_upvalueindex(2));
                    if constexpr (cpp_type<_user_type>::is_pointer) {
                        s.push((ud->*m)());
                    } else {
                        s.push((ud.*m)());
                    }
                    return 1;
                }, 2);
                return 1;
            }
        }
    };

    /*
     * WITH ARGUMENTS AND NO RETURN
     */

    template<typename _user_type, typename _base_type, typename ..._args>
    struct user_method<_user_type, void (_base_type::*)(_args...) const> {
        using method_t = void (_base_type::*)(_args...) const;
        static constexpr const bool read_only = true;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.get<_user_type>(1);
            s.push(&methods[prop]);
            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<_user_type>(lua_upvalueindex(1));
                auto m = s.get<method_t>(lua_upvalueindex(2));
                int arg_idx = 0;
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    (ud->*m)(get_arg<_args>(s, arg_idx)...);
                } else {
                    (ud.*m)(get_arg<_args>(s, arg_idx)...);
                }
                lua_settop(L, 0);
                return 0;
            }, 2);
            return 1;
        }
    };


    template<typename _user_type, typename _base_type, typename ..._args>
    struct user_method<_user_type, void (_base_type::*)(_args...)> {
        using method_t = void (_base_type::*)(_args...);
        static constexpr const bool read_only = false;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            if constexpr (cpp_type<_user_type>::is_const) {
                luaL_error(L, "%s is read only and method %s isn't marked const either.", meta_table<_user_type>::name, prop);
                return 0;
            } else {
                auto ud = s.get<_user_type>(1);
                s.push(&methods[prop]);
                lua_pushcclosure(L, [](lua_State *L) -> int {
                    stack s = stack(state(L));
                    auto ud = s.get<_user_type>(lua_upvalueindex(1));
                    auto m = s.get<method_t>(lua_upvalueindex(2));
                    int arg_idx = 0;
                    if constexpr (cpp_type<_user_type>::is_pointer) {
                        (ud->*m)(get_arg<_args>(s, arg_idx)...);
                    } else {
                        (ud.*m)(get_arg<_args>(s, arg_idx)...);
                    }
                    lua_settop(L, 0);
                    return 0;
                }, 2);
                return 1;
            }
        }
    };

    /*
     * WITH ARGUMENTS AND RETURN
     */

    template<typename _user_type, typename _base_type, typename _r, typename ..._args>
    struct user_method<_user_type, _r (_base_type::*)(_args...) const> {
        using method_t = _r (_base_type::*)(_args...) const;
        static constexpr const bool read_only = true;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.get<_user_type>(1);
            s.push(&methods[prop]);
            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<_user_type>(lua_upvalueindex(1));
                auto m = s.get<method_t>(lua_upvalueindex(2));
                int arg_idx = 0;
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    auto value = (ud->*m)(get_arg<_args>(s, arg_idx)...);
                    lua_pop(L, arg_idx);
                    s.push(value);
                } else {
                    auto value = (ud.*m)(get_arg<_args>(s, arg_idx)...);
                    lua_pop(L, arg_idx);
                    s.push(value);
                }
                return 1;
            }, 2);
            return 1;
        }
    };

    template<typename _user_type, typename _base_type, typename _r, typename ..._args>
    struct user_method<_user_type, _r (_base_type::*)(_args...)> {
        using method_t = _r (_base_type::*)(_args...);
        static constexpr const bool read_only = false;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            if constexpr (cpp_type<_user_type>::is_const) {
                luaL_error(L, "%s is read only and method %s isn't marked const either.", meta_table<_user_type>::name, prop);
                return 0;
            } else {
                auto ud = s.get<_user_type>(1);
                s.push(&methods[prop]);
                lua_pushcclosure(L, [](lua_State *L) -> int {
                    stack s = stack(state(L));
                    auto ud = s.get<_user_type>(lua_upvalueindex(1));
                    auto m = s.get<method_t>(lua_upvalueindex(2));
                    int arg_idx = 0;
                    if constexpr (cpp_type<_user_type>::is_pointer) {
                        auto value = (ud->*m)(get_arg<_args>(s, arg_idx)...);
                        lua_pop(L, arg_idx);
                        s.push(value);
                    } else {
                        auto value = (ud.*m)(get_arg<_args>(s, arg_idx)...);
                        lua_pop(L, arg_idx);
                        s.push(value);
                    }
                    return 1;
                }, 2);
                return 1;
            }
        }
    };
}

#endif //TLW_USER_METHOD_HPP
