//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_SCRIPT_HPP
#define SIMPLELUA_SCRIPT_HPP

#include "stack.hpp"

class Script {
private:
    StackFetcher fetcher;
    StackInserter inserter;
    lua_State *L;
    int index;
public:
    Script(lua_State *L, String scriptPath) : Script(L, scriptPath, false) {}
    Script(lua_State *L, String scriptPath, bool raw) : Script(L) {
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

    Script(lua_State *L) : L(L), fetcher(L), inserter(L) { }

    template<typename T>
    Value<T> *get(const std::string &name) {
        lua_getglobal(L, name.c_str());
        Value<T> *val = fetcher.get<T>(-1);
        lua_pop(L, 1);
        return val;
    }

    template<typename T>
    void set(std::string name, Value<T> val) {
        inserter.insert<T>(val);
        lua_setglobal(L, name.c_str());
    }

    AbstractValue *run() {
        lua_settop(L, 0);
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }

        StackFetcher fetcher(L);
        int returnCount = lua_gettop(L);
        if (!returnCount) {
            return nullptr;
        }
        if (returnCount == 1) {
            return fetcher.get(-1);
        }
        return fetcher.get();
    }

    void dump() {
        int n = lua_gettop(L);
        for (int i = 1; i <= n; ++i) {
            std::string type = luaL_typename(L, i);
            std::cout << "Index: " << i << " Type: " << type << std::endl;
        }
    }
};

#endif //SIMPLELUA_SCRIPT_HPP
