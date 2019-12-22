//
// Created by enijenhuis on 12-12-2019.
//

#ifndef SIMPLELUA_CLASS_H
#define SIMPLELUA_CLASS_H

#include <lua.hpp>
#include <unordered_map>

struct ClassPrototype {
    const std::string &name;
    lua_CFunction constructor;
    std::unordered_map<std::string, lua_CFunction> overloading {};
    std::unordered_map<std::string, lua_CFunction> properties {};
    std::unordered_map<std::string, lua_CFunction> getters {};
    std::unordered_map<std::string, lua_CFunction> setters {};
    std::unordered_map<std::string, lua_CFunction> methods {};

    explicit ClassPrototype(const std::string &name) : ClassPrototype(name, nullptr) {}

    ClassPrototype(const std::string &name, lua_CFunction constr) :
            name(name), constructor(constr) {}
};

class ClassPrototypeBuilder {
private:
    ClassPrototype *prototype;
    bool built = false;
public:
    explicit ClassPrototypeBuilder(const std::string &name) : prototype(new ClassPrototype(name)) {}

    explicit ClassPrototypeBuilder(const std::string &name, lua_CFunction constructor) : prototype(
            new ClassPrototype(name, constructor)) {}

    ~ClassPrototypeBuilder() {
        if (!built)
            delete prototype;
    }

    ClassPrototypeBuilder &constructor(lua_CFunction constr) {
        prototype->constructor = constr;
        return *this;
    }

    ClassPrototypeBuilder &destructor(lua_CFunction f) {
        prototype->overloading["__gc"] = f;
        return *this;
    }

    ClassPrototypeBuilder &method(const std::string &n, lua_CFunction f) {
        prototype->methods[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &property(const std::string &n, lua_CFunction f) {
        prototype->properties[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &getter(const std::string &n, lua_CFunction f) {
        prototype->getters[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &setter(const std::string &n, lua_CFunction f) {
        prototype->setters[n] = f;
        return *this;
    }

    ClassPrototypeBuilder &inverse(lua_CFunction f) {
        prototype->overloading["__unm"] = f;
        return *this;
    }

    ClassPrototypeBuilder &addition(lua_CFunction f) {
        prototype->overloading["__add"] = f;
        return *this;
    }

    ClassPrototypeBuilder &subtraction(lua_CFunction f) {
        prototype->overloading["__sub"] = f;
        return *this;
    }

    ClassPrototypeBuilder &multiplication(lua_CFunction f) {
        prototype->overloading["__mul"] = f;
        return *this;
    }

    ClassPrototypeBuilder &division(lua_CFunction f) {
        prototype->overloading["__div"] = f;
        return *this;
    }

    ClassPrototypeBuilder &modulo(lua_CFunction f) {
        prototype->overloading["__mod"] = f;
        return *this;
    }

    ClassPrototypeBuilder &power(lua_CFunction f) {
        prototype->overloading["__pow"] = f;
        return *this;
    }

    ClassPrototypeBuilder &concat(lua_CFunction f) {
        prototype->overloading["__concat"] = f;
        return *this;
    }

    ClassPrototypeBuilder &equality(lua_CFunction f) {
        prototype->overloading["__eq"] = f;
        return *this;
    }

    ClassPrototypeBuilder &lessThan(lua_CFunction f) {
        prototype->overloading["__lt"] = f;
        return *this;
    }

    ClassPrototypeBuilder &lessThanOrEqual(lua_CFunction f) {
        prototype->overloading["__le"] = f;
        return *this;
    }

    ClassPrototypeBuilder &binaryAnd(lua_CFunction f) {
        prototype->overloading["__band"] = f;
        return *this;
    }

    ClassPrototypeBuilder &binaryOr(lua_CFunction f) {
        prototype->overloading["__bor"] = f;
        return *this;
    }

    ClassPrototypeBuilder &binaryXor(lua_CFunction f) {
        prototype->overloading["__bxor"] = f;
        return *this;
    }

    ClassPrototypeBuilder &binaryInverse(lua_CFunction f) {
        prototype->overloading["__le"] = f;
        return *this;
    }

    ClassPrototypeBuilder &leftShift(lua_CFunction f) {
        prototype->overloading["__le"] = f;
        return *this;
    }

    ClassPrototypeBuilder &rightShift(lua_CFunction f) {
        prototype->overloading["__le"] = f;
        return *this;
    }

    ClassPrototypeBuilder &toString(lua_CFunction f) {
        prototype->overloading["__tostring"] = f;
        return *this;
    }

    ClassPrototype *build() {
        built = true;
        return prototype;
    }
};

#endif //SIMPLELUA_CLASS_H
