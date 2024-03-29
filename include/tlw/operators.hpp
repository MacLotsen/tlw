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

#ifndef TLW_OPERATORS_HPP
#define TLW_OPERATORS_HPP

namespace tlw {

    template<typename _user_type, typename _number_type>
    struct __len {
        using base_type = typename cpp_type<_user_type>::value_type;
        using size_method_t = _number_type (base_type::*)() const;
        static inline size_method_t size_method = nullptr;

        static constexpr int len(lua_State *L) {
            if constexpr (cpp_type<_user_type>::is_pointer) {
                auto ud = stack_traits<_user_type>::get(L, 1);
                lua_settop(L, 0);
                lua_pushnumber(L, (ud->*size_method)());

            } else {
                auto &ud = stack_traits<_user_type>::get(L, 1);
                lua_settop(L, 0);
                lua_pushnumber(L, (ud.*size_method)());
            }
            return 1;
        }
    };

    template<typename _user_type>
    struct __len<_user_type, int> {
        using base_type = typename cpp_type<_user_type>::value_type;
        using size_method_t = int (base_type::*)() const;
        static inline size_method_t size_method = nullptr;

        static constexpr int len(lua_State *L) {
            if constexpr (cpp_type<_user_type>::is_pointer) {
                auto ud = stack_traits<_user_type>::get(L, 1);
                lua_settop(L, 0);
                lua_pushinteger(L, (ud->*size_method)());

            } else {
                auto &ud = stack_traits<_user_type>::get(L, 1);
                lua_settop(L, 0);
                lua_pushinteger(L, (ud.*size_method)());
            }
            return 1;
        }
    };

    template<typename ...>
    struct tostring_f_type {
        static constexpr inline const bool valid = false;
    };

    template<typename ud>
    struct tostring_f_type<const char *(*)(ud)> {
        using return_type = const char *;
        static constexpr inline const bool valid = true;
        static constexpr inline const bool as_pointer = cpp_type<ud>::is_pointer;
    };

    template<typename ud>
    struct tostring_f_type<std::string(*)(ud)> {
        using return_type = std::string;
        static constexpr inline const bool valid = true;
        static constexpr inline const bool as_pointer = cpp_type<ud>::is_pointer;
    };

    template<typename ...>
    struct __tostring {

    };

    template<typename _user_type, typename _tostring_type>
    struct __tostring<_user_type, _tostring_type> {
        static inline _tostring_type fn = nullptr;
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr int tostring(lua_State *L) {
            constexpr bool is_method = method_type<_tostring_type>::valid;
            constexpr bool is_function = tostring_f_type<_tostring_type>::valid;

            auto _ud = stack_traits<_user_type>::get(L, 1);
            const base_type *ud;
            if constexpr (cpp_type<_user_type>::is_pointer) {
                ud = _ud;
            } else {
                ud = &_ud;
            }

            lua_settop(L, 0);

            if constexpr (is_method) {
                stack_traits<typename method_type<_tostring_type>::return_type>::push(L, (ud->*fn)());
            } else if constexpr (is_function) {
                constexpr bool ud_as_p = tostring_f_type<_tostring_type>::as_pointer;
                if constexpr (ud_as_p) {
                    stack_traits<typename tostring_f_type<_tostring_type>::return_type>::push(L, fn(ud));
                } else {
                    stack_traits<typename tostring_f_type<_tostring_type>::return_type>::push(L, fn(*ud));
                }
            } else {
                lua_pushnil(L);
            }

            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __add {
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int add(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                auto r = *ud + *other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                auto r = *ud + other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                auto r = ud + *other;
                stack_traits<decltype(r)>::push(L, r);
            } else {
                auto r = ud + other;
                stack_traits<decltype(r)>::push(L, r);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __sub {
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int sub(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                stack_traits<base_type>::push(L, *ud - *other);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                stack_traits<base_type>::push(L, *ud - other);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                stack_traits<base_type>::push(L, ud - *other);
            } else {
                stack_traits<base_type>::push(L, ud - other);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __mul {
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int mul(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                auto r = *ud * *other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                auto r = *ud * other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                auto r = ud * *other;
                stack_traits<decltype(r)>::push(L, r);
            } else {
                auto r = ud * other;
                stack_traits<decltype(r)>::push(L, r);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __div {
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int div(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                auto r = *ud / *other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                auto r = *ud / other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                auto r = ud / *other;
                stack_traits<decltype(r)>::push(L, r);
            } else {
                auto r = ud / other;
                stack_traits<decltype(r)>::push(L, r);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __mod {
        using base_type = typename cpp_type<_user_type>::value_type;

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int mod(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                auto r = *ud % *other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                auto r = *ud % other;
                stack_traits<decltype(r)>::push(L, r);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                auto r = ud % *other;
                stack_traits<decltype(r)>::push(L, r);
            } else {
                auto r = ud % other;
                stack_traits<decltype(r)>::push(L, r);
            }
            return 1;
        }
    };

    // hold methods + map
//    template<typename _user_type, typename _other_type>
//    struct __pow {
//        using base_type = typename cpp_type<_user_type>::value_type;
//
//        static constexpr bool test(lua_State *L) {
//            return stack_traits<_user_type>::inspect(L, 1)
//                   && stack_traits<_other_type>::inspect(L, 2);
//        }
//
//        static constexpr int mod(lua_State *L) {
//            auto ud = stack_traits<_user_type>::get(L, 1);
//            auto other = stack_traits<_other_type>::get(L, 2);
//            lua_settop(L, 0);
//            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
//                stack_traits<base_type>::push(L, *ud % *other);
//            } else if constexpr (cpp_type<_user_type>::is_pointer) {
//                stack_traits<base_type>::push(L, *ud % other);
//            } else if constexpr (cpp_type<_other_type>::is_pointer) {
//                stack_traits<base_type>::push(L, ud % *other);
//            } else {
//                stack_traits<base_type>::push(L, ud % other);
//            }
//            return 1;
//        }
//    };

    template<typename _user_type, typename _other_type>
    struct __eq {

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int eq(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud == *other);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud == other);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, ud == *other);
            } else {
                stack_traits<bool>::push(L, ud == other);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __lt {

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int lt(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud < *other);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud < other);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, ud < *other);
            } else {
                stack_traits<bool>::push(L, ud < other);
            }
            return 1;
        }
    };

    template<typename _user_type, typename _other_type>
    struct __le {

        static constexpr bool test(lua_State *L) {
            return stack_traits<_user_type>::inspect(L, 1)
                   && stack_traits<_other_type>::inspect(L, 2);
        }

        static constexpr int le(lua_State *L) {
            auto ud = stack_traits<_user_type>::get(L, 1);
            auto other = stack_traits<_other_type>::get(L, 2);
            lua_settop(L, 0);
            if constexpr (cpp_type<_user_type>::is_pointer && cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud <= *other);
            } else if constexpr (cpp_type<_user_type>::is_pointer) {
                stack_traits<bool>::push(L, *ud <= other);
            } else if constexpr (cpp_type<_other_type>::is_pointer) {
                stack_traits<bool>::push(L, ud <= *other);
            } else {
                stack_traits<bool>::push(L, ud <= other);
            }
            return 1;
        }
    };

    template<typename _user_type, typename ...>
    struct __operator {

        static constexpr int to_string(lua_State *L) {
            // Clear the ud from the stack
            lua_settop(L, 0);
            lua_pushstring(L, metatable<_user_type>::name);
            return 1;
        }

        // not generic
        static constexpr int call(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        // not generic
        static constexpr int pairs(lua_State *L) {
            lua_pushnumber(L, 1);
            return 1;
        }

        // not generic
        static constexpr int ipairs(lua_State *L) {
            lua_pushnumber(L, 1);
            return 1;
        }

        static constexpr int unm(lua_State *L) {
            using pt = pop_traits<_user_type>;
            if constexpr (cpp_type<_user_type>::is_pointer) {
                // return value i.o. pointer
                stack_traits<typename cpp_type<_user_type>::value_type>::push(L, -(*pt::pop(L)));
            } else {
                stack_traits<_user_type>::push(L, -pt::pop(L));
            }
            return 1;
        }

    };

    template<typename _user_type, typename _other_type>
    struct __operator<_user_type, _other_type> {

        static constexpr int add(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int sub(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int mul(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int div(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int mod(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int pow(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int concat(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int eq(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int lt(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

        static constexpr int le(lua_State *L) {
            lua_pushnil(L);
            return 1;
        }

    };

}

#endif //TLW_OPERATORS_HPP
