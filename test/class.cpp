//
// Created by enijenhuis on 12-12-2019.
//

#include "test.h"

static const std::string &src =
        "print(example.pname)\n"
        "example.pname = 'new public name'\n"
        "print(example.pname)\n"
        "print(example.name)\n"
        "print(example.concat('asd', 'dsa'))\n"
        "example.name = 'asd'\n"
        "example.print()\n";


class Example {
private:
    std::string name;
public:
    String publicName = "not set";

    Example() : name("example") {}

    explicit Example(std::string n) : name(std::move(n)) {}

    String getName() {
        return name;
    }

    void setName(String n) {
        name = n;
    }

    void concat(String str1, String str2) {
        std::cout << name << str1 << str2 << std::endl;
    }

    void print() {
        std::cout << name << std::endl;
    }
};

lua_CFunction p_public_name = mk_prop(&Example::publicName);
lua_CFunction g_name = mk_getter(&Example::getName);
lua_CFunction s_name = mk_setter(&Example::setName);
lua_CFunction m_concat = mk_method(&Example::concat);
lua_CFunction m_print = mk_method(&Example::print);

int indexer(lua_State *L) {
    String property = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (property == "name") {
        return g_name(L);
//        lua_pushcfunction(L, m_name);
    } else if (property == "concat") {
        lua_pushcclosure(L, m_concat, 1);
    } else if (property == "print") {
        lua_pushcclosure(L, m_print, 1);
    } else if (property == "pname") {
        return p_public_name(L);
    } else {
        lua_pushnil(L);
    }
//    std::cout << "Trying to access name: ";
//    ((T*) lua_touserdata(L, 1))->getName();
//    // method(L);
//    lua_pushstring(L, "Hi");
    return 1;
}

int newIndexer(lua_State *L) {
    String property = lua_tostring(L, 2);
    lua_remove(L, 2);
    if (property == "name") {
        return s_name(L);
    } else if (property == "pname") {
        return p_public_name(L);
    }
    return 0;
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
    lua_pushcfunction(L, indexer);
    lua_settable(L, metatable);
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, newIndexer);
    lua_settable(L, metatable);
    lua_pushliteral(L, "Example");
    lua_pushvalue(L, metatable);
    lua_settable(L, LUA_GLOBALSINDEX);

    Example **ud = (Example **) lua_newuserdata(L, sizeof(Example *));
    *ud = new Example;
    lua_pushvalue(L, metatable);
    lua_setmetatable(L, -2);
    lua_setglobal(L, "example");

    luaL_dostring(L, src.c_str());
}

TEST(MetaTable, testPrototype) {
    ClassPrototype *prototype = ClassPrototypeBuilder("Example")
            .getter("name", g_name)
            .setter("name", s_name)
            .property("pname", p_public_name)
            .method("concat", m_concat)
            .method("print", m_print)
            .build();

    Example e;

    lua.add<Example>(prototype)
            .set("example", &e)
            .src("example", src)
            .call("example");
}
