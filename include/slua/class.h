//
// Created by enijenhuis on 12-12-2019.
//

#ifndef SIMPLELUA_CLASS_H
#define SIMPLELUA_CLASS_H

#include <unordered_map>
#include "function.hpp"

template<typename C, typename R, typename ...ARGS>
using method_t = R (C::*)(ARGS...);

template<typename C, typename R>
using getter_t = R (C::*)();

template<typename C>
using call_t = void (C::*)();

template<typename C, typename ...ARGS>
using setter_t = void (C::*)(ARGS...);




// Keep the class methods in light userdata?

struct ClassPrototype {
    const String &name;
    lua_CFunction constructor;
    std::unordered_map<std::string, lua_CFunction> overloading {};
    std::unordered_map<std::string, lua_CFunction> properties {};
    std::unordered_map<std::string, lua_CFunction> getters {};
    std::unordered_map<std::string, lua_CFunction> setters {};
    std::unordered_map<std::string, lua_CFunction> methods {};

    explicit ClassPrototype(const String &name) : ClassPrototype(name, nullptr) {}

    ClassPrototype(const String &name, lua_CFunction constr) :
            name(name), constructor(constr) {}
};

class ClassPrototypeBuilder {
private:
    ClassPrototype *prototype;
    bool built = false;
public:
    explicit ClassPrototypeBuilder(const String &name) : prototype(new ClassPrototype(name)) {}

    explicit ClassPrototypeBuilder(const String &name, lua_CFunction constructor) : prototype(
            new ClassPrototype(name, constructor)) {}

    ~ClassPrototypeBuilder() {
        if (!built)
            delete prototype;
    }

    ClassPrototypeBuilder &constr(lua_CFunction constr) {
        prototype->constructor = constr;
        return *this;
    }

    ClassPrototypeBuilder &method(const String &n, lua_CFunction f) {
//        prototype->methods.insert(std::make_pair<String, lua_CFunction>(n, f));
        prototype->methods[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &property(const String &n, lua_CFunction f) {
        prototype->properties[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &getter(const String &n, lua_CFunction f) {
        prototype->getters[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &setter(const String &n, lua_CFunction f) {
        prototype->setters[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &overload(const String &n, lua_CFunction f) {
        prototype->overloading[n] = f;
        return *this;
    }

    ClassPrototype *build() {
        built = true;
        return prototype;
    }
};



static int luaMetaConstructor(lua_State *L) {
    int constructed = 0;
    String metatable = lua_tostring(L, lua_upvalueindex(1));
    lua_CFunction constructor = lua_tocfunction(L, lua_upvalueindex(2));
//    lua_settop(L, 0);
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

static int luaMetaOperators(lua_State *L) {
    // TODO operator overloading
    return 0;
}

static int luaMetaIndex(lua_State *L) {
    ClassPrototype *prototype = (ClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    String property = lua_tostring(L, 2);
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
}

static int luaMetaNewIndex(lua_State *L) {
    ClassPrototype *prototype = (ClassPrototype *) lua_touserdata(L, lua_upvalueindex(1));
    String property = lua_tostring(L, 2);
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
}


#endif //SIMPLELUA_CLASS_H
