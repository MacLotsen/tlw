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

#ifndef TLW_STACK_HPP
#define TLW_STACK_HPP

#include "util.hpp"
#include "metatable.hpp"
#include <lua.hpp>
#include <vector>

class Lua;

template<typename ...T>
class TypedStack {

};


template<typename T>
class TypedStack<T> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_islightuserdata(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static T &get(lua_State *L, int i) {
        if (lua_islightuserdata(L, i)) {
            return *((T *) lua_touserdata(L, i));
        } else {
            std::string errorMsg = "Expected a class/struct argument. " + std::string(luaL_typename(L, i)) + " given.";
            throw std::runtime_error(errorMsg);
        }
    }

    inline static T &pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, T *o) {
        lua_pushlightuserdata(L, (void *) o);
    }
};

template<typename T>
class TypedStack<T *> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isuserdata(L, i) || lua_islightuserdata(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static T *get(lua_State *L, int i) {
        T **ptp = (T **) lua_touserdata(L, i);
        return *ptp;
    }

    inline static T *pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, T *o) {
        auto ud = (T**) lua_newuserdata(L, sizeof(T**));
        *ud = o;
    }
};

template<typename T>
class TypedStack<const T> {
};

template<typename T>
class TypedStack<T &> {
};

template<typename T>
class TypedStack<const T &> {
};

template<typename T>
class TypedStack<volatile T &> {
    inline static void push(lua_State *L, volatile T &o) {
        TypedStack<T>::push(L, const_cast<const T &>(o));
    }
};

template<typename T>
class TypedStack<const volatile T &> {
    inline static void push(lua_State *L, const volatile T &o) {
        TypedStack<T>::push(L, const_cast<const T &>(o));
    }
};

template<typename T>
class TypedStack<T &&> {
    inline static void push(lua_State *L, T &&o) {
        TypedStack<T>::push(L, std::forward<T>(o));
    }
};

template<>
class TypedStack<bool> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isboolean(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static bool get(lua_State *L, int i) {
        return lua_toboolean(L, i);
    }

    inline static bool pop(lua_State *L) {
        auto b = get(L, -1);
        lua_pop(L, 1);
        return b;
    }

    inline static void push(lua_State *L, const bool &o) {
        lua_pushboolean(L, o);
    }
};

template<>
class TypedStack<const bool &> {
public:
    inline static void push(lua_State *L, const bool &o) {
        lua_pushboolean(L, o);
    }
};

template<>
class TypedStack<int> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isnumber(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static int get(lua_State *L, int i) {
        return int(lua_tointeger(L, i));
    }

    inline static int pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, int n) {
        lua_pushinteger(L, n);
    }
};

template<>
class TypedStack<unsigned int> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isnumber(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static unsigned int get(lua_State *L, int i) {
        return lua_tointeger(L, i);
    }

    inline static unsigned int pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, unsigned int n) {
        lua_pushinteger(L, n);
    }
};

template<>
class TypedStack<const int &> {
public:
    inline static void push(lua_State *L, const int &n) {
        lua_pushinteger(L, n);
    }
};

template<>
class TypedStack<float> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isnumber(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static float get(lua_State *L, int i) {
        if (lua_isnumber(L, i)) {
            return float(lua_tonumber(L, i));
        }
        lua_pushstring(L, "NaN");
        lua_error(L);
        return 0;
    }

    inline static float pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, float n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const float &> {
public:
    inline static void push(lua_State *L, const float &n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<double> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isnumber(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static double get(lua_State *L, int i) {
        return lua_tonumber(L, i);
    }

    inline static double pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, double n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const double &> {
public:
    inline static void push(lua_State *L, const double &n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const char *> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isstring(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static const char *get(lua_State *L, int i) {
        return lua_tostring(L, i);
    }

    inline static const char *pop(lua_State *L) {
        auto r = get(L, -1);
        lua_pop(L, 1);
        return r;
    }

    inline static void push(lua_State *L, const char *o) {
        lua_pushstring(L, o);
    }
};

template<>
class TypedStack<char *> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isstring(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static char *get(lua_State *L, int i) {
        return const_cast<char *>(TypedStack<const char *>::get(L, i));
    }

    inline static char *pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, const char *o) {
        TypedStack<const char *>::push(L, o);
    }
};

template<>
class TypedStack<std::string> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isstring(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static std::string get(lua_State *L, int i) {
        return TypedStack<const char *>::get(L, i);
    }

    inline static std::string pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, const std::string &o) {
        TypedStack<const char *>::push(L, o.c_str());
    }
};

template<>
class TypedStack<const std::string &> {
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_isstring(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static std::string get(lua_State *L, int i) {
        return TypedStack<const char *>::get(L, i);
    }

    inline static void push(lua_State *L, const std::string &o) {
        TypedStack<const char *>::push(L, o.c_str());
    }
};


template<typename K, typename ...Args>
class TypedStack<std::vector<K, Args...>> {
    typedef std::vector<K, Args...> T;
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_istable(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static T get(lua_State *L, int i) {
        T v;
        lua_pushnil(L);
        while (lua_next(L, i > 0 ? i : i - 1)) {
            v.push_back(TypedStack<typename T::value_type>::pop(L));
        }
        return v;
    }

    inline static T pop(lua_State *L) {
        T v = get(L, -1);
        lua_pop(L, 1);
        return v;
    }

    inline static void push(lua_State *L, const T &v) {
        lua_newtable(L);
        int i = 1;
        for (auto it = v.begin(); it != v.end(); ++it) {
            TypedStack<typename T::value_type>::push(L, *it);
            lua_rawseti(L, -2, i++);
        }
    }
};


template<typename ...Args>
class TypedStack<std::tuple<Args...>> {
    typedef std::tuple<Args...> T;
public:
    inline static bool expect(lua_State *L, int i) {
        return lua_istable(L, i);
    }

    inline static bool expect(lua_State *L) {
        return expect(L, -1);
    }

    inline static T get(lua_State *L, int i) {
        return get(L, i, gen_seq<sizeof...(Args)>());
    }

    template<int ...Is>
    inline static T get(lua_State *L, int i, seq<Is...>) {
        return {fetch<Args>(L, i, Is + 1)...};
    }

    template<typename E>
    inline static E fetch(lua_State *L, int tableIndex, int index) {
        lua_rawgeti(L, tableIndex, index);
        return TypedStack<E>::pop(L);
    }

    inline static T pop(lua_State *L) {
        T values = get(L, -1);
        lua_pop(L, 1);
        return values;
    }

    inline static void push(lua_State *L, const T &t) {
        lua_newtable(L);
        push(L, t, gen_seq<sizeof...(Args)>());
    }

    template<int ...Is>
    inline static void push(lua_State *L, const T &t, seq<Is...>) {
        (..., insert<Args>(L, std::get<Is>(t), -2, Is));
    }

    template<typename E>
    inline static void insert(lua_State *L, E e, int tableIndex, int index) {
        TypedStack<E>::push(L, e);
        lua_rawseti(L, tableIndex, index + 1);
    }
};

template<typename T1, typename T2, typename ...Ts>
class TypedStack<T1, T2, Ts...> {
public:
    template<int ...Is>
    inline static bool expect(lua_State *L, seq<Is...>) {
        return TypedStack<T1>::expect(L, 1) && TypedStack<T2>::expect(L, 2) &&
               (... && TypedStack<Ts>::expect(L, Is + 3));
    }

    inline static bool expect(lua_State *L) {
        return expect(L, gen_seq<sizeof...(Ts)>());
    }

    inline static std::tuple<T1, T2, Ts...> pop(lua_State *L) {
        auto results = get(L);
        lua_settop(L, 0);
        return results;
    }

    inline static void push(lua_State *L, T1 o1, T2 o2, Ts...os) {
        TypedStack<T1>::push(L, o1);
        TypedStack<T2>::push(L, o2);
        (..., TypedStack<Ts>::push(L, os));
    }

    inline static std::tuple<T1, T2, Ts...> get(lua_State *L) {
        return all(L, gen_seq<sizeof...(Ts)>());
    }

    template<int ...Is>
    inline static std::tuple<T1, T2, Ts...> all(lua_State *L, seq<Is...>) {
        return {TypedStack<T1>::get(L, 1), TypedStack<T2>::get(L, 2), TypedStack<Ts>::get(L, Is + 3)...};
    }
};


class Stack {
protected:
    lua_State *L;
public:
    explicit Stack(lua_State *L) : L(L) {}

    template<typename T>
    T pop() {
        return TypedStack<T>::pop(L);
    }

    template<typename Arg1, typename Arg2, typename ...Args>
    std::tuple<Arg1, Arg2, Args...> pop() {
        return TypedStack<Arg1, Arg2, Args...>::pop(L);
    }

    template<typename T>
    void push(T o) {
        TypedStack<T>::push(L, o);
    }

    template<typename Arg1, typename Arg2, typename ...Args>
    void push(Arg1 arg1, Arg2 arg2, Args... args) {
        TypedStack<Arg1, Arg2, Args...>::push(L, arg1, arg2, args...);
    }

    template<typename ...Ts>
    bool expect() {
        return TypedStack<Ts...>::expect(L);
    }
};

#endif //TLW_STACK_HPP
