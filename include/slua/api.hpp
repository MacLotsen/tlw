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
    T src(const std::string &src) {
        if (luaL_loadstring(L, src.c_str())) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<T>::pop(L);
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

    template<typename C>
    Lua &add(const ClassPrototype *klass) {
        if (!MetaTable::metatables.count(&typeid(C *))) {
            MetaTable::metatables[&typeid(C *)] = klass;
        }

        // Get the type of the meta table. Nil if it doesn't exist
        luaL_getmetatable(L, klass->name.c_str());
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

#endif //SIMPLELUA_API_HPP
