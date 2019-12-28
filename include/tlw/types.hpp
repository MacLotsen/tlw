//
// Created by enijenhuis on 11-12-2019.
//

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
        TypedStack<K>::push(L, k);
        lua_gettable(L, -2);
        V v = TypedStack<V>::pop(L);
        lua_pop(L, 1);
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
};

using LuaList = LuaStructure<int>;
using LuaTable = LuaStructure<std::string>;


template<typename ...T>
class LuaFunction : public LuaRef {

};

template<>
class LuaFunction<> : public LuaRef {
public:
    void operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 0, 0)) {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error(err);
        }
    }
};

template<typename R, typename ...Args>
class LuaFunction<R(Args...)> : public LuaRef {
public:
    R operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 1, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<R>::pop(L);
    }
};

template<typename ...Rs, typename ...Args>
class LuaFunction<std::tuple<Rs...>(Args...)> : public LuaRef {
public:
    std::tuple<Rs...> operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), sizeof...(Rs), 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<Rs...>::pop(L);
    }
};

template<typename R>
class LuaFunction<R()> : public LuaRef {
public:
    R operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 1, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<R>::pop(L);
    }
};

#include <iostream>
template<typename ...Rs>
class LuaFunction<std::tuple<Rs...>()> : public LuaRef {
public:
    std::tuple<Rs...> operator()() {
        // TMP
        if (lua_gettop(L)) {
            std::cerr << lua_tostring(L, 1) << std::endl;
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
    void operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 0, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
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
