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

#ifndef TLW_USER_PROP_HPP
#define TLW_USER_PROP_HPP

#include <tlw/detail/util.hpp>
#include <tlw/stack.hpp>
#include <unordered_map>

namespace tlw {

    template<typename _user_type, typename _prop_type, typename ...>
    struct user_prop {
        using base_type = typename cpp_type<_user_type>::value_type;
        using prop_type = _prop_type base_type::*;
        static inline std::unordered_map<std::string_view, prop_type> properties = {};

        static int get(lua_State *L) {
            stack s = stack(state(L));
            auto prop = s.pop<const char *>();
            auto ud = s.pop<_user_type>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                s.push<_prop_type>(ud->*properties[prop]);
            } else {
                s.push<_prop_type>(ud.*properties[prop]);
            }
            return 1;
        }

        static int set(lua_State *L) {
            stack s = stack(state(L));

            auto val = s.pop<_prop_type>();
            auto prop = s.pop<const char *>();
            auto ud = s.pop<_user_type>();
            if constexpr (cpp_type<_user_type>::is_pointer) {
                ud->*properties[prop] = val;
            } else {
                ud.*properties[prop] = val;
            }

            return 0;
        }

        static constexpr int invalid_set(lua_State *L) {
            luaL_error(L, "Error: property '%s' is read only", stack_traits<const char *>::get(L, 2));
            lua_settop(L, 0);
            return 0;
        }
    };

}

#endif //TLW_USER_PROP_HPP
