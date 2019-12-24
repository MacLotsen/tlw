//
// Created by enijenhuis on 23-12-2019.
//
#include <tlw/api.hpp>
#include <tlw/wrapper.h>
#include <tlw/types.hpp>
#include <gtest/gtest.h>
#include <chrono>


static const int iterationCount = 10000000;

struct StandardExample {
    double num;
    StandardExample() : num(1.0) {}

    double get() {
        return num;
    }

    void set(double n) {
        num = n;
    }
};


static int classicGet(lua_State *L) {
    if (lua_gettop(L) != 1) {
        lua_pushstring(L, "Expected a class");
        lua_error(L);
    }
    auto object = *((StandardExample **) lua_touserdata(L, 1));
    lua_settop(L, 0);
    lua_pushnumber(L, object->get());
    return 1;
}

static int classicSet(lua_State *L) {
    if (!lua_isuserdata(L, 1)) {
        lua_pushstring(L, "Not a user datum");
        lua_error(L);
    }
    if (!lua_isnumber(L, 2)) {
        lua_pushstring(L, "Expected a number!");
        lua_error(L);
    }
    auto object = *((StandardExample **) lua_touserdata(L, 1));
    auto n = lua_tonumber(L, 2);
    lua_settop(L, 0);
    object->set(n);
    return 0;
}

class AcceptationTest : public ::testing::Test {
protected:
    Lua *lua = nullptr;
    lua_State *L = nullptr;

    StandardExample *pe = nullptr;

    void SetUp() override {
        pe = new StandardExample;
        lua = new Lua;

        ClassPrototype *prototype = ClassPrototypeBuilder("StandardExample")
                .method("get", mk_function(&StandardExample::get))
                .method("set", mk_function(&StandardExample::set))
                .method("num", mk_property(&StandardExample::num))
                .build();


        lua->add<StandardExample>(prototype)
                .global("object", pe);

        L = luaL_newstate();
        luaL_openlibs(L);

        auto ud = (StandardExample **) lua_newuserdata(L, sizeof(StandardExample **));
        *ud = pe;
        if (luaL_newmetatable(L, "ClassicExample.metatable")) {
            lua_createtable(L, 0, 0);
            lua_pushcfunction(L, classicGet);
            lua_setfield(L, -2, "get");
            lua_pushcfunction(L, classicSet);
            lua_setfield(L, -2, "set");
            lua_setfield(L, -2, "__index");
        }
        lua_setmetatable(L, -2);
        lua_setglobal(L, "object");
    }
    void TearDown() override {
        delete lua;
        lua_close(L);
        delete pe;
    }
};

static const char *getterSetterScript = "local oldVal = object:get()\n"
                            "object:set(oldVal + 50.0)\n";

static const char *propertyScript = "local oldVal = object:num()\n"
                                    "object:num(oldVal + 50.0)\n";

TEST_F(AcceptationTest, testGetterSetter) {
    auto f = lua->src<LuaFunction<>>(getterSetterScript);

    luaL_loadstring(L, getterSetterScript);
    lua_setglobal(L, "module");

    using clock = std::chrono::high_resolution_clock;

    // Sample standard execution time
    auto standard_start = clock::now();
    for (int i = 0; i < iterationCount; ++i)
        f();
    auto standard_end = clock::now();
    auto standard_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(standard_end - standard_start);

    std::cout << std::fixed << std::setprecision(3) << "Standard property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(standard_diff).count() << " milliseconds."
              << std::endl;

    // Sample classic execution time
    auto classic_start = clock::now();
    for (int i = 0; i < iterationCount; ++i) {
        lua_getglobal(L, "module");
        lua_pcall(L, 0, 0, 0);
    }
    auto classic_end = clock::now();
    auto classic_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(classic_end - classic_start);

    std::cout << std::fixed << std::setprecision(3) << "Classic property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(classic_diff).count() << " milliseconds."
              << std::endl;

    ASSERT_LT(double(standard_diff.count()), classic_diff.count() * 1.4)
                                << "Expect standard execution to be at most 2/5 times slower than traditional.";
}

TEST_F(AcceptationTest, testProperty) {
    auto f = lua->src<LuaFunction<>>(propertyScript);

    luaL_loadstring(L, getterSetterScript);
    lua_setglobal(L, "module");

    using clock = std::chrono::high_resolution_clock;

    // Sample standard execution time
    auto standard_start = clock::now();
    for (int i = 0; i < iterationCount; ++i)
        f();
    auto standard_end = clock::now();
    auto standard_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(standard_end - standard_start);

    std::cout << std::fixed << std::setprecision(3) << "Standard property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(standard_diff).count() << " milliseconds."
              << std::endl;

    // Sample classic execution time
    auto classic_start = clock::now();
    for (int i = 0; i < iterationCount; ++i) {
        lua_getglobal(L, "module");
        lua_pcall(L, 0, 0, 0);
    }
    auto classic_end = clock::now();
    auto classic_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(classic_end - classic_start);

    std::cout << std::fixed << std::setprecision(3) << "Classic property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(classic_diff).count() << " milliseconds."
              << std::endl;

    ASSERT_LT(double(standard_diff.count()), classic_diff.count() * 1.4)
                                << "Expect standard execution to be at most 2/5 times slower than traditional.";
}
