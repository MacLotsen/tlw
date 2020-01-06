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

#ifndef TLW_API_HPP
#define TLW_API_HPP

#include "stack.hpp"
#include "metatable.hpp"
#include "types.hpp"
#include <iostream>
#include <lua.hpp>

class Lua : Stack {
private:
    MetaTableRegistry classes;
public:
    Lua() : Lua(luaL_newstate()) {
        luaL_openlibs(L);
    }

    explicit Lua(lua_State *L) : Stack(L), classes(L) {
        lua_pushliteral(L, "classes");
        lua_pushlightuserdata(L, (void*) &classes);
        lua_settable(L, LUA_REGISTRYINDEX);
    }

    ~Lua() {
        lua_close(L);
    }

    LuaFunction<> file(const char *path) {
        if (luaL_loadfile(L, path)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<LuaFunction<>>::pop(L);
    }

    template<typename T>
    T file(const char *path) {
        if (luaL_loadfile(L, path)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<T>::pop(L);
    }

    template<typename T>
    T file(const std::string &path) {
        return this->file<T>(path.c_str());
    }

    LuaFunction<> src(const char *src) {
        if (luaL_loadstring(L, src)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<LuaFunction<>>::pop(L);
    }

    template<typename T>
    T src(const char *src) {
        if (luaL_loadstring(L, src)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<T>::pop(L);
    }

    template<typename T>
    T src(const std::string &src) {
        return this->src<T>(src.c_str());
    }

    template<typename T>
    T get(const std::string &name) {
        lua_getglobal(L, name.c_str());
        return pop<T>();
    }

    template<typename T>
    Lua &set(const std::string &name, T value) {
        push<T>(value);
        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<typename T>
    Lua &set(const std::string &name, T& value) {
        return set(name, &value);
    }

    template<typename T>
    Lua &set(const std::string &name, const T& value) {
        return set(name, &value);
    }

    Lua &set(const std::string &name, lua_CFunction f) {
        lua_register(L, name.c_str(), f);
        return *this;
    }

    template<class C>
    Lua &setObject(const std::string &name, C *object) {
        classes.createObject(object);

        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<class C>
    Lua &setObject(const std::string &name, const C *object) {
        classes.createObject(object);

        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<class C>
    Lua &setObject(const std::string &name, C &object) {
        classes.createObject(&object);

        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<class C>
    Lua &setObject(const std::string &name, const C &object) {
        classes.createObject(&object);

        lua_setglobal(L, name.c_str());
        return *this;
    }

    LuaTable table() {
        lua_createtable(L, 0, 0);
        return LuaTable{L};
    }

    LuaList list() {
        lua_createtable(L, 0, 0);
        return LuaList{L};
    }

    template<typename T, typename P>
    Lua &add(const P *prototype) {
        classes.registerClass<T>(prototype);
        return *this;
    }

    Lua &extend(const char *klass, const char *base) {
        classes.extend(klass, base);
        return *this;
    }

    template<typename T1, typename T2>
    Lua &extend() {
        classes.extend<T1, T2>();
        return *this;
    }
};

#endif //TLW_API_HPP
