//
// Created by enijenhuis on 23-12-2019.
//
#include <slua/api.hpp>
#include <slua/wrapper.h>
#include <slua/types.hpp>
#include <gtest/gtest.h>
#include <chrono>

static const int iterationCount = 10000000;

struct PrettyExample {
    double num;
    PrettyExample() : num(1.0) {}

    double get() {
        return num;
    }

    void set(double n) {
        num = n;
    }
};


static int classicGet(lua_State *L) {
    auto object = *((PrettyExample **) lua_touserdata(L, 1));
    lua_settop(L, 0);
    lua_pushnumber(L, object->get());
    return 1;
}

static int classicSet(lua_State *L) {
    auto object = *((PrettyExample **) lua_touserdata(L, 1));
    auto n = lua_tonumber(L, 2);
    lua_settop(L, 0);
    object->set(n);
    return 0;
}

class PrettyAcceptationTest : public ::testing::Test {
protected:
    Lua *lua = nullptr;
    lua_State *L = nullptr;

    PrettyExample *pe = nullptr;

    void SetUp() override {
        pe = new PrettyExample;
        lua = new Lua;

        PrettyClassPrototype *prototype = PrettyClassPrototypeBuilder("PrettyExample")
                .getter("get", mk_function(&PrettyExample::get))
                .setter("set", mk_function(&PrettyExample::set))
                .property("num", mk_property(&PrettyExample::num))
                .build();


        lua->add<PrettyExample>(prototype)
                .global("object", pe);

        L = luaL_newstate();
        luaL_openlibs(L);

        auto ud = (PrettyExample **) lua_newuserdata(L, sizeof(PrettyExample **));
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

static const char *getterSetterScript = "local oldVal = object.get\n"
                            "object.set = oldVal + 50.0\n";

static const char *propertyScript = "local oldVal = object.num\n"
                                    "object.num = oldVal + 50.0\n";

static const char *classicGetterSetterScript = "local oldVal = object:get()\n"
                                   "object:set(oldVal + 50.0)\n";

TEST_F(PrettyAcceptationTest, testGetterSetter) {
    auto f = lua->src<LuaFunction<>>(getterSetterScript);

    luaL_loadstring(L, classicGetterSetterScript);
    lua_setglobal(L, "module");

    using clock = std::chrono::high_resolution_clock;

    // Sample pretty execution time
    auto pretty_start = clock::now();
    for (int i = 0; i < iterationCount; ++i)
        f();
    auto pretty_end = clock::now();
    auto pretty_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(pretty_end - pretty_start);

    std::cout << std::fixed << std::setprecision(3) << "Pretty property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(pretty_diff).count() << " milliseconds."
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

    ASSERT_LT(pretty_diff.count() / 10, classic_diff.count())
                                << "Expect pretty execution to be at most 10 times slower than traditional.";
}

TEST_F(PrettyAcceptationTest, testProperty) {
    auto f = lua->src<LuaFunction<>>(propertyScript);

    luaL_loadstring(L, classicGetterSetterScript);
    lua_setglobal(L, "module");

    using clock = std::chrono::high_resolution_clock;

    // Sample pretty execution time
    auto pretty_start = clock::now();
    for (int i = 0; i < iterationCount; ++i)
        f();
    auto pretty_end = clock::now();
    auto pretty_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(pretty_end - pretty_start);

    std::cout << std::fixed << std::setprecision(3) << "Pretty property time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(pretty_diff).count() << " milliseconds."
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

    ASSERT_LT(pretty_diff.count() / 7, classic_diff.count())
                                << "Expect pretty execution to be at most 7 times slower than traditional.";
}
