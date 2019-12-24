//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_METATABLE_H
#define SIMPLELUA_METATABLE_H

#include "class.h"
#include <lua.hpp>

static void luaCreateMetaTable(lua_State *L, const ClassPrototype *klass);
static void luaCreateMetaTable(lua_State *L, const PrettyClassPrototype *klass);

class MetaTable {
public:
    inline static std::unordered_map<const std::type_info *, const ClassPrototype*> metaTables = {};
    inline static std::unordered_map<const std::type_info *, const PrettyClassPrototype*> prettyTables = {};

    template<typename T>
    static void createObject(lua_State *L, T *object) {
        T **ud = (T **) lua_newuserdata(L, sizeof(T **));
        *ud = object;
        if (metaTables.count(&typeid(T*)))
            luaCreateMetaTable(L, metaTables[&typeid(T*)]);
        else if (prettyTables.count(&typeid(T*)))
            luaCreateMetaTable(L, prettyTables[&typeid(T*)]);
        lua_setmetatable(L, -2);
    }
};

static int luaMetaConstructor(lua_State *L) {
    int constructed = 0;
    std::string metatable = lua_tostring(L, lua_upvalueindex(1));
    lua_CFunction constructor = lua_tocfunction(L, lua_upvalueindex(2));
    constructed = constructor(L);
    if (!constructed) {
        lua_pushstring(L, "Constructor did not return an object.");
        lua_error(L);
    }
    for (int i = 1; i <= constructed; ++i) {
        luaL_getmetatable(L, metatable.c_str());
        lua_setmetatable(L, i);
    }
    return constructed;
}

static int luaMetaIndex(lua_State *L) {
    PrettyClassPrototype *prototype = (PrettyClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    std::string property = lua_tostring(L, 2);
    lua_pop(L, 1);

    for (auto kv: prototype->properties) {
        if (property == kv.first) {
            return kv.second(L);
        }
    }

    for (auto kv: prototype->getters) {
        if (property == kv.first) {
            return kv.second(L);
        }
    }

    for (auto kv: prototype->methods) {
        if (property == kv.first) {
            lua_pushcclosure(L, kv.second, 1);
            return 1;
        }
    }

    lua_pushstring(L, ("No such property " + property).c_str());
    lua_error(L);
    return 0;
}

static int luaMetaNewIndex(lua_State *L) {
    PrettyClassPrototype *prototype = (PrettyClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    std::string property = lua_tostring(L, 2);
    lua_remove(L, 2);

    for (auto kv: prototype->properties) {
        if (property == kv.first) {
            return kv.second(L);
        }
    }

    for (auto kv: prototype->setters) {
        if (property == kv.first) {
            return kv.second(L);
        }
    }

    lua_pushstring(L, ("No such property " + property).c_str());
    lua_error(L);
    return 0;
}

static void luaCreateMetaTable(lua_State *L, const PrettyClassPrototype *klass) {
    if (luaL_newmetatable(L, klass->name.c_str())) {
        // Target metatable
        int metatable = lua_gettop(L);

        // First set the value for all closures (indexer e.t.c.)
        lua_pushlightuserdata(L, (void *) klass);
        int prototype = lua_gettop(L);

        // Create anonymous metatable (Deny access for lua)
        lua_pushliteral(L, "__metatable");
        lua_pushnil(L);
        lua_settable(L, metatable);

        // Set constructor if needed
        if (klass->constructor) {
            lua_pushstring(L, klass->name.c_str());
            lua_pushcfunction(L, klass->constructor);
            lua_pushcclosure(L, luaMetaConstructor, 2);
            lua_setglobal(L, klass->name.c_str());
        }

        // Set all operator overloading
        for (auto kv: klass->operators) {
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

        // Pop the prototype and leave the meta table on top of the stack
        lua_pop(L, 1);
    }
}

static void luaCreateMetaTable(lua_State *L, const ClassPrototype *klass) {
    if (luaL_newmetatable(L, klass->name.c_str())) {
        // Target metatable
        int metatable = lua_gettop(L);

        // Create anonymous meta table (Deny access for lua)
        lua_pushliteral(L, "__metatable");
        lua_pushnil(L);
        lua_settable(L, metatable);

        // Set constructor if needed
        if (klass->constructor) {
            lua_pushstring(L, klass->name.c_str());
            lua_pushcfunction(L, klass->constructor);
            lua_pushcclosure(L, luaMetaConstructor, 2);
            lua_setglobal(L, klass->name.c_str());
        }

        // Set all operator overloading
        for (auto kv: klass->operators) {
            lua_pushstring(L, kv.first.c_str());
            lua_pushcfunction(L, kv.second);
            lua_settable(L, metatable);
        }

        if (!klass->methods.empty()) {
            // Set the methods
            lua_createtable(L, 0, 0);
            for (auto kv: klass->methods) {
                lua_pushcfunction(L, kv.second);
                lua_setfield(L, -2, kv.first.c_str());
            }
            lua_setfield(L, -2, "__index");
        }
    }
}

#endif //SIMPLELUA_METATABLE_H
