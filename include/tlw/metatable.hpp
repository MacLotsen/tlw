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

#ifndef TLW_METATABLE_H
#define TLW_METATABLE_H

#include "errors.hpp"
#include "prototype.hpp"
#include <vector>
#include <lua.hpp>

static void luaCreateMetaTable(lua_State *L, const ClassPrototype *klass);

static void luaCreateMetaTable(lua_State *L, const PrettyClassPrototype *klass);

class MetaTableRegistry {
private:
    std::unordered_map<const std::type_info *, const char *> metaTableRegistry = {};
    lua_State *L;
public:
    explicit MetaTableRegistry(lua_State *L) : L(L) {}

    template<typename D, typename B, typename ...Os>
    void extend() {
        luaL_getmetatable(L, metaTableRegistry[&typeid(D *)]);
        int thisMetatable = lua_gettop(L);
        _extend<D, B, Os...>("__index");
        lua_settop(L, thisMetatable);
        _extend<D, B, Os...>("__newindex");
        lua_settop(L, 0);
    }

    template<typename C, typename P>
    void registerClass(P *prototype) {
        luaCreateMetaTable(L, prototype);
        metaTableRegistry[&typeid(C *)] = prototype->name;
        lua_pop(L, 1);
    }

    template<typename T>
    void createObject(T *object) {
        auto ud = (T **) lua_newuserdata(L, sizeof(T **));
        *ud = object;
        if (metaTableRegistry.find(&typeid(T *)) != metaTableRegistry.end()) {
            luaL_getmetatable(L, metaTableRegistry[&typeid(T *)]);
            lua_setmetatable(L, -2);
        } else {
            throw UnsupportedScriptTypeError("No such class");
        }
    }

    template<typename T>
    void createObject(const T *object) {
        auto ud = (const T **) lua_newuserdata(L, sizeof(T **));
        *ud = object;
        if (metaTableRegistry.find(&typeid(const T *)) != metaTableRegistry.end()) {
            luaL_getmetatable(L, metaTableRegistry[&typeid(const T *)]);
            lua_setmetatable(L, -2);
        } else {
            throw UnsupportedScriptTypeError("No such class");
        }
    }

private:
    template<typename T>
    void _extend(const char * metaField) {
        // No classes left to bind
    }

    template<typename D, typename B, typename ...Os>
    void _extend(const char * metaField) {
        int t;
        lua_getfield(L, -1, metaField);
        if (lua_isnil(L, -1)) {
            // Create an index wrapper
            lua_pop(L, 1);
            lua_createtable(L, 0, 0);
            lua_pushvalue(L, -1);
            lua_setfield(L, -3, metaField);
        }
        if (lua_istable(L, -1)) {
            luaL_getmetatable(L, metaTableRegistry[&typeid(B *)]);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -3);

            _extend<B, Os...>(metaField);
        } else {
            throw UnsupportedPrototypeExtensionError("Inheritance only works with table indexes");
        }
    }
};


static int luaMetaConstructor(lua_State *L) {
    int constructed = 0;
    const char *metatable = lua_tostring(L, lua_upvalueindex(1));
    lua_CFunction constructor = lua_tocfunction(L, lua_upvalueindex(2));
    constructed = constructor(L);
    if (!constructed) {
        lua_pushstring(L, "MetaConstructor: did not return an object.");
        lua_error(L);
    }
    for (int i = 1; i <= constructed; ++i) {
        luaL_getmetatable(L, metatable);
        lua_setmetatable(L, i);
    }
    return constructed;
}

static int luaMetaIndex(lua_State *L) {
    auto prototype = (PrettyClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    const char *property = lua_tostring(L, 2);
    lua_pop(L, 1);

    if (prototype->properties.find(property) != prototype->properties.end())
        return prototype->properties[property](L);

    if (prototype->getters.find(property) != prototype->getters.end())
        return prototype->getters[property](L);

    if (prototype->methods.find(property) != prototype->methods.end()) {
        lua_pushcclosure(L, prototype->methods[property], 1);
        return 1;
    }

    lua_pushfstring(L, "PrettyClass: No such getter '%s'", property);
    lua_error(L);
    return 0;
}

static int luaMetaNewIndex(lua_State *L) {
    auto prototype = (PrettyClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    const char *property = lua_tostring(L, 2);
    lua_remove(L, 2);

    if (prototype->properties.find(property) != prototype->properties.end())
        return prototype->properties[property](L);

    if (prototype->setters.find(property) != prototype->setters.end())
        return prototype->setters[property](L);

    lua_pushfstring(L, "PrettyClass: No such setter '%s'", property);
    lua_error(L);
    return 0;
}

static void luaCreateMetaTable(lua_State *L, const PrettyClassPrototype *klass) {
    if (luaL_newmetatable(L, klass->name)) {
        // Target metatable
        int metatable = lua_gettop(L);

        // First set the value for all closures (indexer e.t.c.)
        lua_pushlightuserdata(L, (void *) klass);
        int prototype = lua_gettop(L);

        // Create anonymous metatable (Deny access for lua by default)
        lua_pushliteral(L, "__metatable");
        lua_pushfstring(L, "%s is private", klass->name);
        lua_settable(L, metatable);

        // Set constructor if needed
        if (klass->constructor) {
            lua_pushstring(L, klass->name);
            lua_pushcfunction(L, klass->constructor);
            lua_pushcclosure(L, luaMetaConstructor, 2);
            lua_setglobal(L, klass->name);
        }

        // Set all operator overloading
        for (auto kv: klass->operators) {
            lua_pushstring(L, kv.first.data());
            lua_pushcfunction(L, kv.second);
            lua_settable(L, metatable);
        }

        if (!klass->properties.empty() || !klass->getters.empty() || !klass->methods.empty()) {
            lua_pushliteral(L, "__index");
            lua_pushvalue(L, prototype);
            lua_pushcclosure(L, luaMetaIndex, 1);
            lua_settable(L, metatable);
        }

        if (!klass->properties.empty() || !klass->setters.empty()) {
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
    if (luaL_newmetatable(L, klass->name)) {
        // Target metatable
        int metatable = lua_gettop(L);

        // Create anonymous meta table (Deny access for lua)
        lua_pushliteral(L, "__metatable");
        lua_pushfstring(L, "%s is private", klass->name);
        lua_settable(L, metatable);

        // Set constructor if needed
        if (klass->constructor) {
            lua_pushstring(L, klass->name);
            lua_pushcfunction(L, klass->constructor);
            lua_pushcclosure(L, luaMetaConstructor, 2);
            lua_setglobal(L, klass->name);
        }

        // Set all operator overloading
        for (auto kv: klass->operators) {
            lua_pushstring(L, kv.first.data());
            lua_pushcfunction(L, kv.second);
            lua_settable(L, metatable);
        }

        if (!klass->methods.empty()) {
            // Set the methods
            lua_createtable(L, 0, 0);
            for (auto kv: klass->methods) {
                lua_pushcfunction(L, kv.second);
                lua_setfield(L, -2, kv.first.data());
            }
            lua_setfield(L, -2, "__index");
        }
    }
}

#endif //TLW_METATABLE_H
