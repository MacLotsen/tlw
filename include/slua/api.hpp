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
#include "class.h"
#include "metatable.h"
#include "wrapper.h"

class Lua {
private:
    lua_State *L;
    std::unordered_map<std::string, Script *> scripts;
public:
    Lua() : L(luaL_newstate()) {
        luaL_openlibs(L);
    }

    ~Lua() {
        for (auto kv:scripts) {
            delete kv.second;
        }
        lua_close(L);
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

    template<class C>
    Lua &set(String name, C *object) {
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
