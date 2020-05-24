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

#ifndef TLW__LUA_HPP
#define TLW__LUA_HPP

namespace tlw {
    struct lua : public global_table {

        lua() : global_table(state()) {}

        lua(state s) : global_table(std::move(s)) {}

        function<void()> src(const char *source_code) {
            if (luaL_loadstring(L, source_code)) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            auto val = stack_traits<function<void()>>::get(L, -1);
            lua_pop(L, 1);
            return val;
        }

        template<typename _fn_type>
        function<_fn_type> src(const char *source_code) {
            if (luaL_loadstring(L, source_code)) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            auto val = stack_traits<function<_fn_type>>::get(L, -1);
            lua_pop(L, 1);
            return val;
        }

        function<void()> file(const char *file_name) {
            if (luaL_loadfile(L, file_name)) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            auto val = stack_traits<function<void()>>::get(L, -1);
            lua_pop(L, 1);
            return val;
        }

        template<typename _fn_type>
        function<_fn_type> file(const char *file_name) {
            if (luaL_loadfile(L, file_name)) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            auto val = stack_traits<function<_fn_type>>::get(L, -1);
            lua_pop(L, 1);
            return val;
        }
    };
}

#endif //TLW__LUA_HPP
