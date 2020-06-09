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

    template<typename ...>
    struct user_method {

    };

    template<typename _user_type, typename _base_type>
    struct user_method<_user_type, void (_base_type::*)() const> {
        using method_t = void (_base_type::*)() const;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return lua_gettop(L) == 0;
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            if constexpr (cpp_type<_user_type>::is_pointer) {
                (ud->*m)();
            } else {
                (ud.*m)();
            }
            return 0;
        }
    };

    template<typename _user_type, typename _base_type>
    struct user_method<_user_type, void (_base_type::*)()> {
        using method_t = void (_base_type::*)();
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return lua_gettop(L) == 0;
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            if constexpr (cpp_type<_user_type>::is_const) {
                return -1;
            } else {
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    (ud->*m)();
                } else {
                    (ud.*m)();
                }
            }
            return 0;
        }
    };

    /*
     * WITH RETURN
     */

    template<typename _user_type, typename _base_type, typename _r>
    struct user_method<_user_type, _r (_base_type::*)() const> {
        using method_t = _r (_base_type::*)() const;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return lua_gettop(L) == 0;
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            if constexpr (cpp_type<_user_type>::is_pointer) {
                s.push((ud->*m)());
            } else {
                s.push((ud.*m)());
            }
            return 1;
        }
    };

    template<typename _user_type, typename _base_type, typename _r>
    struct user_method<_user_type, _r (_base_type::*)()> {
        using method_t = _r (_base_type::*)();
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return lua_gettop(L) == 0;
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            if constexpr (cpp_type<_user_type>::is_const) {
                return -1;
            } else {
                if constexpr (cpp_type<_user_type>::is_pointer) {
                    s.push((ud->*m)());
                } else {
                    s.push((ud.*m)());
                }
            }
            return 1;
        }
    };

    /*
     * WITH ARGUMENTS AND NO RETURN
     */

    template<typename _user_type, typename _base_type, typename ..._args>
    struct user_method<_user_type, void (_base_type::*)(_args...) const> {
        using method_t = void (_base_type::*)(_args...) const;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return test(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr bool test(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            auto seq = gen_seq<sizeof...(_args)>();
            invoke(s, ud, m, seq);
            return 0;
        }

        template<int ...Is>
        static void invoke(stack &s, _user_type ud, method_t m, seq<Is...>) {
            auto args = s.grab<typename remove_ref<_args>::type...>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                (ud->*m)(std::get<Is>(args)...);
            } else {
                (ud.*m)(std::get<Is>(args)...);
            }
        }
    };


    template<typename _user_type, typename _base_type, typename ..._args>
    struct user_method<_user_type, void (_base_type::*)(_args...)> {
        using method_t = void (_base_type::*)(_args...);
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return test(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr bool test(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            if constexpr (cpp_type<_user_type>::is_const) {
                s.clear();
                return -1;
            } else {
                auto seq = gen_seq<sizeof...(_args)>();
                invoke(s, ud, m, seq);
            }
            return 0;
        }

        template<int ...Is>
        static void invoke(stack &s, _user_type ud, method_t m, seq<Is...>) {
            auto args = s.grab<typename remove_ref<_args>::type...>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                (ud->*m)(std::get<Is>(args)...);
            } else {
                (ud.*m)(std::get<Is>(args)...);
            }
        }
    };

    /*
     * WITH ARGUMENTS AND RETURN
     */

    template<typename _user_type, typename _base_type, typename _r, typename ..._args>
    struct user_method<_user_type, _r (_base_type::*)(_args...) const> {
        using method_t = _r (_base_type::*)(_args...) const;
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return test(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr bool test(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            auto m = methods[prop];
            auto seq = gen_seq<sizeof...(_args)>();
            return invoke(s, ud, m, seq);
        }

        template<int ...Is>
        static int invoke(stack &s, _user_type ud, method_t m, seq<Is...>) {
            auto args = s.grab<typename remove_ref<_args>::type...>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                auto value = (ud->*m)(std::get<Is>(args)...);
                s.push(value);
            } else {
                auto value = (ud.*m)(std::get<Is>(args)...);
                s.push(value);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _base_type, typename _r, typename ..._args>
    struct user_method<_user_type, _r (_base_type::*)(_args...)> {
        using method_t = _r (_base_type::*)(_args...);
        static inline std::unordered_map<std::string_view, method_t> methods = {};

        static constexpr bool test(lua_State *L) {
            return test(L, gen_seq<sizeof...(_args)>());
        }

        template<int ...Is>
        static constexpr bool test(lua_State *L, seq<Is...>) {
            return (... && stack_traits<_args>::inspect(L, Is + 1));
        }

        static constexpr int provide(stack &s, _user_type ud, const char *prop) {
            if constexpr (cpp_type<_user_type>::is_const) {
                return -1;
            } else {
                auto seq = gen_seq<sizeof...(_args)>();
                auto m = methods[prop];
                invoke(s, ud, m, seq);
            }
            return 1;
        }

        template<int ...Is>
        static int invoke(stack &s, _user_type ud, method_t m, seq<Is...>) {
            auto args = s.grab<typename remove_ref<_args>::type...>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                auto value = (ud->*m)(std::get<Is>(args)...);
                s.push(value);
            } else {
                auto value = (ud.*m)(std::get<Is>(args)...);
                s.push(value);
            }
            return 1;
        }
    };
}

#endif //TLW_USER_METHOD_HPP
