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
#include "method.h"
#include "getter.h"
#include "setter.h"
#include "property.h"

#define newtable(L) (lua_newtable(L), lua_gettop(L))

class Lua {
private:
    lua_State *L;
    std::unordered_map<std::string, Script *> scripts;
    std::unordered_map<const std::type_info*, std::string> metatables;
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

    template<class C>
    Lua &set(String name, C* object) {
        C **ud = (C **) lua_newuserdata(L, sizeof(C *));
        *ud = object;
        luaL_getmetatable(L, metatables[&typeid(C)].c_str());
        lua_setmetatable(L, -2);
        lua_setglobal(L, name.c_str());
        return *this;
    }

    template<typename C>
    Lua &add(const ClassPrototype *klass) {
        // First set the value for all closures (indexer e.t.c.)
        lua_pushlightuserdata(L, (void*) klass);
        int prototype = lua_gettop(L);

        // Target metatable
        luaL_newmetatable(L, klass->name.c_str());
        int metatable = lua_gettop(L);
        metatables[&typeid(C)] = klass->name;

        // Create anonymous metatable (Deny access for lua)
        lua_pushliteral(L, "__metatable");
        lua_newtable(L);
        lua_settable(L, metatable);

        // Set constructor if needed
        if (klass->constructor) {
            lua_pushliteral(L, "__call");
            lua_pushstring(L, klass->name.c_str());
            lua_pushcfunction(L, klass->constructor);
            lua_pushcclosure(L, luaMetaConstructor, 2);
            lua_settable(L, metatable);
        }

        // Set all operator overloading
        for (auto kv: klass->overloading) {
            lua_pushstring(L, kv.first.c_str());
            lua_pushcfunction(L, kv.second);
            lua_settable(L, metatable);
        }

        if (klass->properties.size() || klass->getters.size() || klass->methods.size()) {
            lua_pushliteral(L, "__index");
            lua_pushvalue(L, prototype);
            lua_pushcclosure(L, luaMetaIndex, 1);
            lua_settable(L, metatable);
        }

        if (klass->properties.size() || klass->setters.size()) {
            lua_pushliteral(L, "__newindex");
            lua_pushvalue(L, prototype);
            lua_pushcclosure(L, luaMetaNewIndex, 1);
            lua_settable(L, metatable);
        }

        lua_settop(L, 0);
        return *this;
    }
};

#endif //SIMPLELUA_API_HPP
