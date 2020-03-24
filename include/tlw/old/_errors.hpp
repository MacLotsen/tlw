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

#ifndef TLW__ERRORS_HPP
#define TLW__ERRORS_HPP

#include <exception>
#include <lua.hpp>

class LuaError : public std::exception {
private:
    const char * msg;
public:
    explicit LuaError(const char * msg) : msg(msg), std::exception() {}
    explicit LuaError(lua_State *L) : std::exception() {
        msg = lua_tostring(L, -1);
        lua_settop(L, 0);
    }

    const char * what() const noexcept {
        return msg;
    }
};

class ScriptError : public LuaError {
public:
    ScriptError(lua_State *L) : LuaError(L) {}
};

class ScriptFunctionError : public LuaError {
public:
    ScriptFunctionError(lua_State *L) : LuaError(L) {}
};

class ScriptFunctionArgumentError : public LuaError {
public:
    ScriptFunctionArgumentError(const char *msg) : LuaError(msg) {}
};

class UnsupportedScriptTypeError : public LuaError {
public:
    UnsupportedScriptTypeError(const char * msg) : LuaError(msg) {}
};

class UnsupportedPrototypeExtensionError : public LuaError {
public:
    UnsupportedPrototypeExtensionError(const char * msg) : LuaError(msg) {}
};

#endif //TLW__ERRORS_HPP
