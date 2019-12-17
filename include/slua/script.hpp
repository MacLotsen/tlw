//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_SCRIPT_HPP
#define SIMPLELUA_SCRIPT_HPP

#include "stack.hpp"

class Script {
private:
    lua_State *L;
    int index;
    LuaStack stack;
public:
    Script(lua_State *L, const String& scriptPath) : Script(L, scriptPath, false) {}
    Script(lua_State *L, const String& scriptPath, bool raw) : Script(L) {
        if (raw ? (luaL_loadstring(L, scriptPath.c_str())) : (luaL_loadfile(L, scriptPath.c_str()))) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    Script(const char *scriptPath) : Script(luaL_newstate()) {
        luaL_openlibs(L);
        if (luaL_loadfile(L, scriptPath)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    Script(lua_State *L) : L(L), stack(L) { }

    AbstractValue *run() {
        lua_settop(L, 0);
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }

        int returnCount = lua_gettop(L);
        if (!returnCount) {
            return nullptr;
        }
        if (returnCount == 1) {
            return stack.pop();
        }

        List l;
        std::stack<AbstractValue *> rargs = stack.getArgs();

        while(!rargs.empty()) {
            l.push_back(rargs.top());
            rargs.pop();
        }

        return new Value<List>(l);
    }
};

#endif //SIMPLELUA_SCRIPT_HPP
