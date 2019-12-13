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
        "example.print()\n"
        "local example2 = Example('example 2')\n"
        "print(example2.name)\n"
        "print(Example)\n"
        "example2 = nil\n"
        "";


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

Example *example2;

Example* exampleConstructor(String s) {
    return example2 = new Example(s);
}

int luaGc(lua_State *L) {
    Example **ud = (Example**) lua_touserdata(L, 1);

    std::cout << "Example " << (*ud)->getName() << " escaped. It should be collected." << std::endl;

    return 0;
}


TEST(MetaTable, testPrototype) {
    {
        Lua _lua;
        ClassPrototype *prototype = ClassPrototypeBuilder("Example")
                .constr(mk_cfunc(&exampleConstructor))
                .getter("name", mk_getter(&Example::getName))
                .setter("name", mk_setter(&Example::setName))
                .property("pname", mk_prop(&Example::publicName))
                .method("concat", mk_method(&Example::concat))
                .method("print", mk_method(&Example::print))
//                .overload("__gc", luaGc)
                .build();

        Example e;

        _lua.add<Example>(prototype)
                .set("example", &e)
                .src("example", src)
                .call("example");
    }
    ASSERT_EQ("example 2", example2->getName());
    delete example2;
}
