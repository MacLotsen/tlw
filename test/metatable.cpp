//
// Created by enijenhuis on 12-12-2019.
//

#include <gtest/gtest.h>
#include <lua.hpp>
#include <utility>
#include <slua/types.hpp>
#include <slua/class.h>

static const std::string& src =
//        "print(Example)\n"
//        "example:test()\n"
//        "example.test = 'asd'\n"
        "print(example.name)\n"
        "print(example.concat('asd', 'dsa'))\n"
        "example.print()\n";
//        "print(example.jibberish)"
        ;

class Example {
private:
    std::string name;
public:
    Example() : name("example") {}

    explicit Example(std::string n) : name(std::move(n)) {}

    String getName() {
        return name;
    }

    void concat(String str1, String str2) {
        std::cout << name << str1 << str2 << std::endl;
    }

    void print() {
        std::cout << name << std::endl;
    }
};


lua_CFunction m_name = mk_method(&Example::getName);
lua_CFunction m_concat = mk_method(&Example::concat);
lua_CFunction m_print = mk_method(&Example::print);

template<class T>
int indexer(lua_State *L) {
    String property = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (property == "name") {
        return m_name(L);
//        lua_pushcfunction(L, m_name);
    } else if (property == "concat") {
        lua_pushcclosure(L, m_concat, 1);
    } else if (property == "print") {
        return m_print(L);
    } else {
        lua_pushnil(L);
    }
//    std::cout << "Trying to access name: ";
//    ((T*) lua_touserdata(L, 1))->getName();
//    // method(L);
//    lua_pushstring(L, "Hi");
    return 1;
}

TEST(MetaTable, testCreation) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
//    lua_newtable(L);
//    int methods = lua_gettop(L);
//    lua_pushliteral(L, "test");
//    lua_pushcfunction(L, test<Example>);
//    lua_settable(L, methods);
    lua_newtable(L);
    int metatable = lua_gettop(L);
    lua_pushliteral(L, "__index");
//    lua_pushvalue(L, methods);
    lua_pushcfunction(L, indexer<Example>);
    lua_settable(L, metatable);
    lua_pushliteral(L, "Example");
    lua_pushvalue(L, metatable);
    lua_settable(L, LUA_GLOBALSINDEX);

    Example ** ud = (Example**) lua_newuserdata(L, sizeof(Example*));
    *ud = new Example;
    lua_pushvalue(L, metatable);
    lua_setmetatable(L, -2);
    lua_setglobal(L, "example");

    luaL_dostring(L, src.c_str());
}
