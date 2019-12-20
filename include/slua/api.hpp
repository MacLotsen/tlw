//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_API_HPP
#define SIMPLELUA_API_HPP

#include "stack.hpp"
#include "metatable.h"
#include <lua.hpp>

class Lua : Stack {
public:
    Lua() : Stack(luaL_newstate()) {
        luaL_openlibs(L);
    }

    ~Lua() {
        lua_close(L);
    }

    template<typename T>
    T file(const std::string &path) {
        if (luaL_loadfile(L, path.c_str())) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<T>::pop(L);
    }

    template<typename T>
    T src(const std::string &name, const std::string &src) {
        if (luaL_loadstring(L, src.c_str())) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<T>::pop(L);
    }

    Lua &add(const std::string &name, lua_CFunction f) {
        lua_register(L, name.c_str(), f);
        return *this;
    }

    template<class C>
    Lua &set(const std::string &name, C *object) {
        C **ud = (C **) lua_newuserdata(L, sizeof(C **));
        *ud = object;
        luaL_getmetatable(L, MetaTable::metatables[&typeid(C*)].c_str());
        lua_setmetatable(L, -2);
        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<typename C>
    Lua &add(const ClassPrototype *klass) {
        if (!MetaTable::metatables.count(&typeid(C *))) {
            MetaTable::metatables[&typeid(C *)] = klass->name;
        }
        luaCreateMetaTable(L, klass);
        return *this;
    }
};

#endif //SIMPLELUA_API_HPP
