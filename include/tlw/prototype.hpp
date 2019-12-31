//
// Created by enijenhuis on 12-12-2019.
//

#ifndef TLW_CLASS_H
#define TLW_CLASS_H

#include <lua.hpp>
#include <unordered_map>
#include <utility>


using function_mapping = std::unordered_map<std::string_view, lua_CFunction>;

struct ClassPrototype {
    const char * name;
    lua_CFunction constructor;
    function_mapping operators{};
    function_mapping methods{};

    explicit ClassPrototype(const char *name) : ClassPrototype(name, nullptr) {}

    ClassPrototype(const char * name, lua_CFunction constr) :
            name(name), constructor(constr) {}
};

struct PrettyClassPrototype {
    const char * name;
    lua_CFunction constructor;
    function_mapping operators{};
    function_mapping properties{};
    function_mapping getters{};
    function_mapping setters{};
    function_mapping methods{};

    explicit PrettyClassPrototype(const char *name) : PrettyClassPrototype(name, nullptr) {}

    PrettyClassPrototype(const char * name, lua_CFunction constr) :
            name(name), constructor(constr) {}
};

class ClassPrototypeBuilder {
private:
    ClassPrototype *prototype;
    bool built = false;
public:
    explicit ClassPrototypeBuilder(const char *name) : prototype(new ClassPrototype(name)) {}

    explicit ClassPrototypeBuilder(const char *name, lua_CFunction constructor) : prototype(
            new ClassPrototype(name, constructor)) {}

    ~ClassPrototypeBuilder() {
        if (!built)
            delete prototype;
    }

    ClassPrototypeBuilder& constructor(lua_CFunction constr) {
        prototype->constructor = constr;
        return *this;
    }

    ClassPrototypeBuilder &destructor(lua_CFunction f) {
        prototype->operators["__gc"] = f;
        return *this;
    }

    ClassPrototypeBuilder &method(const char *n, lua_CFunction f) {
        prototype->methods[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &inverse(lua_CFunction f) {
        prototype->operators["__unm"] = f;
        return *this;
    }

    ClassPrototypeBuilder &addition(lua_CFunction f) {
        prototype->operators["__add"] = f;
        return *this;
    }

    ClassPrototypeBuilder &subtraction(lua_CFunction f) {
        prototype->operators["__sub"] = f;
        return *this;
    }

    ClassPrototypeBuilder &multiplication(lua_CFunction f) {
        prototype->operators["__mul"] = f;
        return *this;
    }

    ClassPrototypeBuilder &division(lua_CFunction f) {
        prototype->operators["__div"] = f;
        return *this;
    }

    ClassPrototypeBuilder &modulo(lua_CFunction f) {
        prototype->operators["__mod"] = f;
        return *this;
    }

    ClassPrototypeBuilder &power(lua_CFunction f) {
        prototype->operators["__pow"] = f;
        return *this;
    }

    ClassPrototypeBuilder &concat(lua_CFunction f) {
        prototype->operators["__concat"] = f;
        return *this;
    }

    ClassPrototypeBuilder &equality(lua_CFunction f) {
        prototype->operators["__eq"] = f;
        return *this;
    }

    ClassPrototypeBuilder &lessThan(lua_CFunction f) {
        prototype->operators["__lt"] = f;
        return *this;
    }

    ClassPrototypeBuilder &lessThanOrEqual(lua_CFunction f) {
        prototype->operators["__le"] = f;
        return *this;
    }

    ClassPrototypeBuilder &toString(lua_CFunction f) {
        prototype->operators["__tostring"] = f;
        return *this;
    }

    ClassPrototype *build() {
        built = true;
        return prototype;
    }
};

class PrettyClassPrototypeBuilder {
private:
    PrettyClassPrototype *prototype;
    bool built = false;
public:
    explicit PrettyClassPrototypeBuilder(const char *name) : prototype(new PrettyClassPrototype(name)) {}

    explicit PrettyClassPrototypeBuilder(const char *name, lua_CFunction constructor) : prototype(
            new PrettyClassPrototype(name, constructor)) {}

    ~PrettyClassPrototypeBuilder() {
        if (!built)
            delete prototype;
    }

    PrettyClassPrototypeBuilder &constructor(lua_CFunction constr) {
        prototype->constructor = constr;
        return *this;
    }

    PrettyClassPrototypeBuilder &destructor(lua_CFunction f) {
        prototype->operators["__gc"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &method(const char *n, lua_CFunction f) {
        prototype->methods[n] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &property(const char *n, lua_CFunction f) {
        prototype->properties[n] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &getter(const char *n, lua_CFunction f) {
        prototype->getters[n] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &setter(const char *n, lua_CFunction f) {
        prototype->setters[n] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &inverse(lua_CFunction f) {
        prototype->operators["__unm"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &addition(lua_CFunction f) {
        prototype->operators["__add"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &subtraction(lua_CFunction f) {
        prototype->operators["__sub"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &multiplication(lua_CFunction f) {
        prototype->operators["__mul"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &division(lua_CFunction f) {
        prototype->operators["__div"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &modulo(lua_CFunction f) {
        prototype->operators["__mod"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &power(lua_CFunction f) {
        prototype->operators["__pow"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &concat(lua_CFunction f) {
        prototype->operators["__concat"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &equality(lua_CFunction f) {
        prototype->operators["__eq"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &lessThan(lua_CFunction f) {
        prototype->operators["__lt"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &lessThanOrEqual(lua_CFunction f) {
        prototype->operators["__le"] = f;
        return *this;
    }

    PrettyClassPrototypeBuilder &toString(lua_CFunction f) {
        prototype->operators["__tostring"] = f;
        return *this;
    }

    PrettyClassPrototype *build() {
        built = true;
        return prototype;
    }
};

#endif //TLW_CLASS_H
