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

#ifndef TLW_METHOD_TRAITS_HPP
#define TLW_METHOD_TRAITS_HPP

namespace tlw {

    template<typename ...>
    struct method_traits {

    };

//    template<typename _user_type, typename _r, typename ..._args>
//    struct method_traits<_r (_user_type::*)(_args...)> {
//        using method_type = _r (_user_type::*)(_args...);
//        static inline std::unordered_map<std::string_view, method_type> methods = {};
//
//        static int invoke(lua_State *L) {
//            if constexpr (const_type<_user_type>::valid) {
//                lua_pushstring(L, "Error: user type is read only");
//                lua_error(L);
//            } else {
//                stack s = stack(state(L));
//                auto ud = s.pick<_user_type>(1);
//                auto r = ud->*method_ptr(s.pick<_args>(1)...);
//                s.push(r);
//            }
//            return 1;
//        }
//    };
//
//    template<typename _user_type, typename _r, typename ..._args>
//    struct method_traits<_r (_user_type::*)(_args...) const> {
//        using method_type = _r (_user_type::*)(_args...) const;
//        static inline std::unordered_map<std::string_view, method_type> methods = {};
//
//        static int invoke(lua_State *L) {
//            stack s = stack(state(L));
//            auto ud = s.pick<_user_type>(1);
//            auto r = ud->*method_ptr(s.pick<_args>(1)...);
//            s.push(r);
//            return 1;
//        }
//    };
//
//    template<typename _user_type, typename ..._args>
//    struct method_traits<void (_user_type::*)(_args...) const> {
//        using method_type = void (_user_type::*)(_args...) const;
//        static inline std::unordered_map<std::string_view, method_type> methods = {};
//
//        static int invoke(lua_State *L) {
//            stack s = stack(state(L));
//            auto ud = s.pick<_user_type>(1);
//            ud->*method_ptr(s.pick<_args>(1)...);
//            return 0;
//        }
//    };

    template<typename _user_type>
    struct method_traits<void (_user_type::*)() const> {
        using method_type = void (_user_type::*)() const;
        static constexpr const bool read_only = true;
        static inline std::unordered_map<std::string_view, method_type> methods = {};

        static int provide(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.get<const _user_type *>(1);
            s.push(&methods[prop]);
            lua_pushcclosure(L, [](lua_State *L) -> int {
                stack s = stack(state(L));
                auto ud = s.get<const _user_type *>(lua_upvalueindex(1));
                auto m = s.get<method_type>(lua_upvalueindex(2));
                (ud->*m)();
            }, 2);
            return 1;
        }
    };

//    template<typename _user_type, typename ..._args>
//    struct method_traits<void (_user_type::*)(_args...)> {
//        using method_type = void (_user_type::*)(_args...);
//        static inline std::unordered_map<std::string_view, method_type> methods = {};
//
//        static int invoke(lua_State *L) {
//            if constexpr (const_type<_user_type>::valid) {
//                lua_pushstring(L, "Error: user type is read only");
//                lua_error(L);
//            } else {
//                stack s = stack(state(L));
//                auto ud = s.pick<_user_type>(1);
//                ud->*method_ptr(s.pick<_args>(1)...);
//            }
//            return 0;
//        }
//    };

}

#endif //TLW_METHOD_TRAITS_HPP
