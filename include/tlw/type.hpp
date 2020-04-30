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

#ifndef TLW_TYPE_HPP
#define TLW_TYPE_HPP

#include <lua.hpp>

namespace tlw {

    template<typename _c_type, int _lua_type>
    struct type_set {
        using type = _c_type;
        static constexpr int value = _lua_type;
    };

    using nil_t =       type_set<void *, LUA_TNIL>;
    using none_t =      type_set<void, LUA_TNONE>;
    using bool_t =      type_set<bool, LUA_TBOOLEAN>;
    using number_t =    type_set<lua_Number, LUA_TNUMBER>;
    using integer_t =   type_set<lua_Integer, LUA_TNUMBER>;
    using string_t =    type_set<const char *, LUA_TSTRING>;
    using table_t =     type_set<reference, LUA_TTABLE>;
    using function_t =  type_set<reference, LUA_TFUNCTION>;
    using anonymous_t = type_set<reference, LUA_TNIL>;
    using cfunction_t = type_set<lua_CFunction, LUA_TFUNCTION>;

    template<typename _user_type>
    using user_data_t = type_set<_user_type, LUA_TUSERDATA>;

    template<typename _user_type>
    using light_user_data_t = type_set<_user_type, LUA_TLIGHTUSERDATA>;

    using thread_t = type_set<lua_State *, LUA_TTHREAD>;

}

#endif //TLW_TYPE_HPP
