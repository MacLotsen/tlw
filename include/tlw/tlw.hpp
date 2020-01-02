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
public:
    Lua() : Stack(luaL_newstate()) {
        luaL_openlibs(L);
    }

    explicit Lua(lua_State *L) : Stack(L) {}

    ~Lua() {
        lua_close(L);
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
    T global(const std::string &name) {
        lua_getglobal(L, name.c_str());
        return pop<T>();
    }

    template<typename T>
    Lua &global(const std::string &name, T value) {
        push<T>(value);
        lua_setglobal(L, name.c_str());
        return *this;
    }

    Lua &global(const std::string &name, lua_CFunction f) {
        lua_register(L, name.c_str(), f);
        return *this;
    }

    template<class C>
    Lua &global(const std::string &name, C *object) {
        push(object);
        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<typename K>
    LuaStructure<K> table() {
        lua_createtable(L, 0, 0);
        return LuaStructure<K>(L);
    }

    template<typename C>
    Lua &add(const ClassPrototype *klass) {
        if (!MetaTable::metaTables.count(&typeid(C *))) {
            MetaTable::metaTables[&typeid(C *)] = klass;
        }

        // Get the type of the meta table. Nil if it doesn't exist for this lua instance
        luaL_getmetatable(L, klass->name);
        int t = lua_type(L, -1);
        lua_pop(L, 1);

        if (t == LUA_TNIL) {
            // Create meta table for this lua instance directly for e.g. constructor creation
            luaCreateMetaTable(L, klass);
            lua_pop(L, 1);
        } else {
            std::cerr << "Warning: Class '" << klass->name << "'already created." << std::endl;
        }

        return *this;
    }

    template<typename C>
    Lua &add(const PrettyClassPrototype *klass) {
        if (!MetaTable::prettyTables.count(&typeid(C *))) {
            MetaTable::prettyTables[&typeid(C *)] = klass;
        }

        // Get the type of the meta table. Nil if it doesn't exist for this lua instance
        luaL_getmetatable(L, klass->name);
        int t = lua_type(L, -1);
        lua_pop(L, 1);

        if (t == LUA_TNIL) {
            // Create meta table for this lua instance directly for e.g. constructor creation
            luaCreateMetaTable(L, klass);
            lua_pop(L, 1);
        } else {
            std::cerr << "Warning: Class '" << klass->name << "'already created." << std::endl;
        }

        return *this;
    }
};

#endif //TLW_API_HPP
