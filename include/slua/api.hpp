//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_API_HPP
#define SIMPLELUA_API_HPP

#include <unordered_map>
#include <lua.hpp>
#include "types.hpp"
#include "value.hpp"
#include "script.hpp"
#include "function.hpp"
#include "class.h"

class Lua {
private:
    lua_State *L;
    std::unordered_map<std::string, Script *> scripts;
    std::unordered_map<const std::type_info *, std::string> metaTables;
public:
    Lua() : L(luaL_newstate()) {
        luaL_openlibs(L);
    }

    Lua &file(const std::string &name, const std::string &path) {
        scripts[name] = new Script(L, path.c_str());
        return *this;
    }

    Lua &src(const std::string &name, const std::string &src) {
        scripts[name] = new Script(L, src.c_str(), true);
        return *this;
    }

    AbstractValue *call(const std::string &name) {
        return scripts[name]->run();
    }

    Lua &add(const std::string &name, lua_CFunction f) {
        lua_register(L, name.c_str(), f);
        return *this;
    }

    template<class T>
    Lua &add(const std::string &name) {
        metaTables[&typeid(T)] = name;
        luaL_newmetatable(L, name.c_str());
        return *this;
    }

    template<class T>
    Lua &add(const std::string &name, lua_CFunction f) {
        if (!metaTables.count(&typeid(T))) {
            throw std::runtime_error("No such class registered '" + std::string(typeid(T).name()) + "'");
        }
        luaL_getmetatable(L, metaTables[&typeid(T)].c_str());
        lua_pushcfunction(L, f);
        lua_pushstring(L, name.c_str());
        lua_settable(L, -3);
        return *this;
    }
};

#endif //SIMPLELUA_API_HPP
