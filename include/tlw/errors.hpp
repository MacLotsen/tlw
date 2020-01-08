//
// Created by enijenhuis on 8-1-2020.
//

#ifndef TLW_ERRORS_HPP
#define TLW_ERRORS_HPP

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

#endif //TLW_ERRORS_HPP
