//
// Created by enijenhuis on 11-12-2019.
//

#ifndef TLW_API_HPP
#define TLW_API_HPP

#include "stack.hpp"
#include "metatable.hpp"
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

    template<typename C>
    Lua &add(const ClassPrototype *klass) {
        if (!MetaTable::metaTables.count(&typeid(C *))) {
            MetaTable::metaTables[&typeid(C *)] = klass;
        }

        // Get the type of the meta table. Nil if it doesn't exist for this lua instance
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

    template<typename C>
    Lua &add(const PrettyClassPrototype *klass) {
        if (!MetaTable::prettyTables.count(&typeid(C *))) {
            MetaTable::prettyTables[&typeid(C *)] = klass;
        }

        // Get the type of the meta table. Nil if it doesn't exist for this lua instance
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

#endif //TLW_API_HPP