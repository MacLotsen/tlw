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

#ifndef TLW_TYPES_HPP
#define TLW_TYPES_HPP

#include "stack.hpp"

class LuaRef {
    friend TypedStack<LuaRef>;
protected:
    lua_State *L;
    int index = -1;
public:
    LuaRef(lua_State *L) : L(L) {
        index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    LuaRef(const LuaRef &ref) : L(ref.L) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref.index);
        index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    ~LuaRef() {
        luaL_unref(L, LUA_REGISTRYINDEX, index);
    }
};

template<typename K>
class LuaStructure : public LuaRef {
public:
    template<typename V>
    V get(K k) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        V v = getOfStack<V>(-2, k);
        lua_pop(L, 2);
        return v;
    }

    template<typename V>
    void set(K k, V v) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        TypedStack<K>::push(L, k);
        TypedStack<V>::push(L, v);
        lua_settable(L, -3);
        lua_pop(L, 1);
    }

    template<typename ...Vs>
    std::tuple<Vs...> all(K keys...) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        int i = lua_gettop(L);
        std::tuple<Vs...> r = {getOfStack<Vs>(i, keys)...};
        lua_settop(L, i - 1);
        return r;
    }

    template<typename ...Vs>
    void setAll(std::pair<K, Vs> ...values) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        int i = lua_gettop(L);
        (..., setOnStack<Vs>(i, values.first, values.second));
        lua_pop(L, 1);
    }

    void unset(K k) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        lua_pushnil(L);
        lua_settable(L, -2);
        lua_pop(L, 1);
    }
private:
    template<typename V>
    V getOfStack(int index, K k) {
        TypedStack<K>::push(L, k);
        lua_gettable(L, index);
        return TypedStack<V>::get(L, -1);
    }

    template<typename V>
    void setOnStack(int index, K k, V v) {
        TypedStack<K>::push(L, k);
        TypedStack<V>::push(L, v);
        lua_settable(L, index);
    }
};

using LuaList = LuaStructure<int>;
using LuaTable = LuaStructure<const char *>;


template<typename ...T>
class LuaFunction : public LuaRef {
};

template<>
class LuaFunction<> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<> &f) : LuaRef(f) {}
    void operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 0, 0)) {
            throw ScriptFunctionError(L);
        }
    }
};

template<typename R, typename ...Args>
class LuaFunction<R(Args...)> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<R(Args...)> &f) : LuaRef(f) {}
    R operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 1, 0)) {
            throw ScriptFunctionError(L);
        }
        return TypedStack<R>::pop(L);
    }
};

template<typename ...Rs, typename ...Args>
class LuaFunction<std::tuple<Rs...>(Args...)> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<std::tuple<Rs...>(Args...)> &f) : LuaRef(f) {}
    std::tuple<Rs...> operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), sizeof...(Rs), 0)) {
            throw ScriptFunctionError(L);
        }
        return TypedStack<Rs...>::pop(L);
    }
};

template<typename R>
class LuaFunction<R()> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<R()> &f) : LuaRef(f) {}
    R operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 1, 0)) {
            throw ScriptFunctionError(L);
        }
        return TypedStack<R>::pop(L);
    }
};

#include <iostream>
template<typename ...Rs>
class LuaFunction<std::tuple<Rs...>()> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<std::tuple<Rs...>()> &f) : LuaRef(f) {}
    std::tuple<Rs...> operator()() {
        if (lua_gettop(L)) {
            throw ScriptFunctionArgumentError(lua_tostring(L, -1));
        }
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, sizeof...(Rs), 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<Rs...>::pop(L);
    }
};

template<typename ...Args>
class LuaFunction<void(Args...)> : public LuaRef {
public:
    LuaFunction(lua_State *L) : LuaRef(L) {}
    LuaFunction(const LuaFunction<void(Args...)> &f) : LuaRef(f) {}
    void operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 0, 0)) {
            throw ScriptFunctionError(L);
        }
    }
};


template<typename K>
class TypedStack<LuaStructure<K>> {
    using T = LuaStructure<K>;
public:
    inline static T get(lua_State *L, int i) {
        lua_pushvalue(L, i);
        return T{L};
    }

    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<>
class TypedStack<LuaRef> {
public:
    inline static void push(lua_State *L, const LuaRef &r) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r.index);
    }
};


template<typename R, typename ...Args>
class TypedStack<LuaFunction<R(Args...)>> {
    typedef LuaFunction<R(Args...)> T;
public:
    inline static T get(lua_State *L, int i) {
        lua_pushvalue(L, i);
        return T{L};
    }

    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<typename ...Args>
class TypedStack<LuaFunction<void(Args...)>> {
    typedef LuaFunction<void(Args...)> T;
public:
    inline static T get(lua_State *L, int i) {
        lua_pushvalue(L, i);
        return T{L};
    }

    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<typename R>
class TypedStack<LuaFunction<R()>> {
    typedef LuaFunction<R()> T;
public:
    inline static T get(lua_State *L, int i) {
        lua_pushvalue(L, i);
        return T{L};
    }

    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<>
class TypedStack<LuaFunction<>> {
    typedef LuaFunction<> T;
public:
    inline static T get(lua_State *L, int i) {
        lua_pushvalue(L, i);
        return T{L};
    }

    inline static T pop(lua_State *L) {
        return T{L};
    }
};

#endif //TLW_TYPES_HPP
