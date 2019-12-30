//
// Created by enijenhuis on 30-12-2019.
//

#ifndef TLW_CLASSIC_H
#define TLW_CLASSIC_H

#include "examples.h"

class ClassicBenchmarkRunner : public BenchmarkRunner {
protected:
    lua_State *L = nullptr;
public:
    ~ClassicBenchmarkRunner() {
        if (L)
            lua_close(L);
    }

    void prepare(const char *script) override {
        L = luaL_newstate();
        luaL_openlibs(L);
        luaL_loadstring(L, script);
        lua_setglobal(L, "module");
    }

    void run() override {
        lua_getglobal(L, "module");
        if (lua_pcall(L, 0, 0, 0)) {
            std::cerr << lua_tostring(L, -1) << std::endl;
            throw std::runtime_error("Error calling script");
        }
    }
};

int classicPropertyGet(lua_State *L) {
    std::string prop = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (prop == "number") {
#ifdef STRICT_ARGUMENTS
        if (lua_gettop(L) != 1) {
            lua_pushstring(L, "Expected a class");
            lua_error(L);
        }
        if (!lua_isuserdata(L, 1)) {
            lua_pushstring(L, "Not a user datum");
            lua_error(L);
        }
#endif
        auto object = *((PropertyExample **) lua_touserdata(L, 1));
        lua_settop(L, 0);
        lua_pushnumber(L, object->number);
        return 1;
    }
    lua_pushstring(L, "No such property to get");
    lua_error(L);
    return 0;
}

int classicPropertySet(lua_State *L) {
    std::string prop = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (prop == "number") {
#ifdef STRICT_ARGUMENTS
        if (lua_gettop(L) != 2) {
            lua_pushstring(L, "Expected a class");
            lua_error(L);
        }
        if (!lua_isuserdata(L, 1)) {
            lua_pushstring(L, "Not a user datum");
            lua_error(L);
        }
        if (!lua_isnumber(L, 2)) {
            lua_pushstring(L, "Expected a number!");
            lua_error(L);
        }
#endif
        auto object = *((PropertyExample **) lua_touserdata(L, 1));
        auto n = lua_tonumber(L, 2);
        lua_settop(L, 0);
        object->number = n;
        return 0;
    }
    lua_pushstring(L, "No such property to set");
    lua_error(L);
    return 0;
}

int classicGet(lua_State *L) {
    std::string prop = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (prop == "number") {
#ifdef STRICT_ARGUMENTS
        if (lua_gettop(L) != 1) {
            lua_pushstring(L, "Expected a class");
            lua_error(L);
        }
        if (!lua_isuserdata(L, 1)) {
            lua_pushstring(L, "Not a user datum");
            lua_error(L);
        }
#endif
        auto object = *((PropertyExample **) lua_touserdata(L, 1));
        lua_settop(L, 0);
        lua_pushnumber(L, object->getNumber());
        return 1;
    }
    lua_pushstring(L, "No such property to get");
    lua_error(L);
    return 0;
}

int classicSet(lua_State *L) {
    std::string prop = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (prop == "number") {
#ifdef STRICT_ARGUMENTS
        if (lua_gettop(L) != 2) {
            lua_pushstring(L, "Expected a class");
            lua_error(L);
        }
        if (!lua_isuserdata(L, 1)) {
            lua_pushstring(L, "Not a user datum");
            lua_error(L);
        }
        if (!lua_isnumber(L, 2)) {
            lua_pushstring(L, "Expected a number!");
            lua_error(L);
        }
#endif
        auto object = *((PropertyExample **) lua_touserdata(L, 1));
        auto n = lua_tonumber(L, 2);
        lua_settop(L, 0);
        object->setNumber(n);
        return 0;
    }
    lua_pushstring(L, "No such property to set");
    lua_error(L);
    return 0;
}

int classicNormalGet(lua_State *L) {
#ifdef STRICT_ARGUMENTS
    if (lua_gettop(L) != 1) {
        lua_pushstring(L, "Expected a class");
        lua_error(L);
    }
    if (!lua_isuserdata(L, 1)) {
        lua_pushstring(L, "Not a user datum");
        lua_error(L);
    }
#endif
    auto object = *((PropertyExample **) lua_touserdata(L, 1));
    lua_settop(L, 0);
    lua_pushnumber(L, object->getNumber());
    return 1;
}

int classicNormalSet(lua_State *L) {
#ifdef STRICT_ARGUMENTS
    if (lua_gettop(L) != 2) {
        lua_pushstring(L, "Expected a class");
        lua_error(L);
    }
    if (!lua_isuserdata(L, 1)) {
        lua_pushstring(L, "Not a user datum");
        lua_error(L);
    }
    if (!lua_isnumber(L, 2)) {
        lua_pushstring(L, "Expected a number!");
        lua_error(L);
    }
#endif
    auto object = *((PropertyExample **) lua_touserdata(L, 1));
    auto n = lua_tonumber(L, 2);
    lua_settop(L, 0);
    object->setNumber(n);
    return 0;
}

class ClassicPropertyBenchmark : public ClassicBenchmarkRunner {
private:
    PropertyExample *example;
public:
    ~ClassicPropertyBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ClassicBenchmarkRunner::prepare(script);
        example = new PropertyExample;
        auto ud = (PropertyExample **) lua_newuserdata(L, sizeof(PropertyExample **));
        *ud = example;
        if (luaL_newmetatable(L, "PropertySet")) {
            lua_pushcfunction(L, classicPropertySet);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, classicPropertyGet);
            lua_setfield(L, -2, "__index");
        }
        lua_setmetatable(L, -2);
        lua_setglobal(L, "example");
    }
};


class ClassicGetterSetterBenchmark : public ClassicBenchmarkRunner {
private:
    PropertyExample *example;
public:
    ~ClassicGetterSetterBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ClassicBenchmarkRunner::prepare(script);
        example = new PropertyExample;
        auto ud = (PropertyExample **) lua_newuserdata(L, sizeof(PropertyExample **));
        *ud = example;
        if (luaL_newmetatable(L, "PropertySet")) {
            lua_pushcfunction(L, classicSet);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, classicGet);
            lua_setfield(L, -2, "__index");
        }
        lua_setmetatable(L, -2);
        lua_setglobal(L, "example");
    }
};

class ClassicGetterSetterAsTableBenchmark : public ClassicBenchmarkRunner {
private:
    PropertyExample *example;
public:
    ~ClassicGetterSetterAsTableBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ClassicBenchmarkRunner::prepare(script);
        example = new PropertyExample;
        auto ud = (PropertyExample **) lua_newuserdata(L, sizeof(PropertyExample **));
        *ud = example;
        if (luaL_newmetatable(L, "PropertySet")) {
            lua_createtable(L, 0, 0);
            lua_pushcfunction(L, classicNormalSet);
            lua_setfield(L, -2, "setNumber");
            lua_pushcfunction(L, classicNormalGet);
            lua_setfield(L, -2, "getNumber");
            lua_setfield(L, -2, "__index");
        }
        lua_setmetatable(L, -2);
        lua_setglobal(L, "example");
    }
};

class ClassicScriptResultsBenchmark : public ClassicBenchmarkRunner {
public:
    void run() override {
        lua_getglobal(L, "module");
        if (lua_pcall(L, 0, 3, 0)) {
            std::cerr << lua_tostring(L, -1) << std::endl;
            throw std::runtime_error("Error calling script");
        }
        if (!lua_isnumber(L, 1) || !lua_isboolean(L, 2) || !lua_isstring(L, 3)) {
            throw std::runtime_error("Expected 3 returns. number, bool, string");
        }
        auto n = lua_tonumber(L, 1);
        auto b = lua_toboolean(L, 2);
        auto s = lua_tostring(L, 3);
        lua_settop(L, 0);
    }
};

#endif //TLW_CLASSIC_H
