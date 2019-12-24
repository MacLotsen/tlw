//
// Created by enijenhuis on 16-12-2019.
//

#ifndef SIMPLELUA_WRAPPER_H
#define SIMPLELUA_WRAPPER_H

#include <lua.hpp>
#include <functional>
#include "stack.hpp"
#include "util.h"

using function_none_to_none_t = void (*)();

template<typename R>
using function_none_to_one_t = R (*)();

template<typename ...Args>
using function_many_to_none_t = void (*)(Args...);

template<typename R, typename ...Args>
using function_many_to_one_t = R (*)(Args...);

template<class C, typename T>
using class_property_t = T C::*;

template<typename C>
using class_none_to_none_t = void (C::*)();

template<typename C, typename R>
using class_none_to_one_t = R (C::*)();

template<typename C, typename ...Args>
using class_many_to_none_t = void (C::*)(Args...);

template<typename C, typename R, typename ...Args>
using class_many_to_one_t = R (C::*)(Args...);

template<typename LAST>
static std::string typesToString() {
    return typeid(LAST).name();
}

template<typename FIRST, typename SECOND, typename ...Args>
static std::string typesToString() {
    return std::string(typeid(FIRST).name()) + ", " + typesToString<SECOND, Args...>();
}

template<typename ...Args>
static void errorUnmatchedArguments(lua_State *L) {
    std::string errorMessage = "Expected: " + typesToString<Args...>();
    lua_pushstring(L, errorMessage.c_str());
    lua_error(L);
}

static std::string getTypesOnStack(lua_State *L) {
    std::string scratch = "";
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i) {
        scratch += luaL_typename(L, i);
        if (i != n) {
            scratch += ", ";
        }
    }
    return scratch;
}

static void expectNoArguments(lua_State *L) {
    if (lua_gettop(L)) {
        lua_pushstring(L, (std::string("Expected no arguments. ") + getTypesOnStack(L) + " given.").c_str());
        lua_error(L);
    }
}

template<typename ...Args>
static void expectArguments(lua_State *L) {
    // TODO reimplement inspector
//    if (!LuaStack(L).expect<Args...>()) {
//        errorUnmatchedArguments<Args...>(L);
//    }
    if (lua_gettop(L) != sizeof...(Args)) {
        errorUnmatchedArguments<Args...>(L);
    }
}

template<typename T>
static T *getClass(lua_State *L) {
    T *klass = *((T **) lua_touserdata(L, 1));
    lua_remove(L, 1);
    return klass;
}

template<typename T>
static T *getClassByUpValue(lua_State *L) {
    return *((T **) lua_touserdata(L, lua_upvalueindex(1)));
}

template<typename T>
class LuaCFunctionInvoker {};

template<typename ...Args>
class LuaCFunctionInvoker<void(Args...)> {
    using T = void(*)(Args...);
public:
    inline static void invoke(lua_State *L, T f) {
        invoke(L, f, gen_seq<sizeof...(Args)>());
    }
    template<int ...Is>
    inline static void invoke(lua_State *L, T f, seq<Is...>) {
        f(TypedStack<Args>::get(L, Is + 1)...);
        lua_settop(L, 0);
    }
};

template<typename R, typename ...Args>
class LuaCFunctionInvoker<R(Args...)> {
    using T = R(*)(Args...);
public:
    inline static R invoke(lua_State *L, T f) {
        return invoke(L, f, gen_seq<sizeof...(Args)>());
    }
    template<int ...Is>
    inline static R invoke(lua_State *L, T f, seq<Is...>) {
        R r = f(TypedStack<Args>::get(L, Is + 1)...);
        lua_settop(L, 0);
        TypedStack<R>::push(L, r);
        return r;
    }
};

template<typename T>
class LuaCMethodInvoker {};

template<typename C, typename ...Args>
class LuaCMethodInvoker<C(void(Args...))> {
    using T = void(C::*)(Args...);
public:
    inline static void invoke(lua_State *L, C *klass, T f) {
        invoke(L, klass, f, gen_seq<sizeof...(Args)>());
    }
    template<int ...Is>
    inline static void invoke(lua_State *L, C *klass, T f, seq<Is...>) {
        (klass->*f)(TypedStack<Args>::get(L, Is + 1)...);
        lua_settop(L, 0);
    }
};

template<typename C, typename R, typename ...Args>
class LuaCMethodInvoker<C(R(Args...))> {
    using T = R(C::*)(Args...);
public:
    inline static R invoke(lua_State *L, C *klass, T f) {
        return invoke(L, klass, f, gen_seq<sizeof...(Args)>());
    }
    template<int ...Is>
    inline static R invoke(lua_State *L, C *klass, T f, seq<Is...>) {
        R r = (klass->*f)(TypedStack<Args>::get(L, Is + 1)...);
        lua_settop(L, 0);
        TypedStack<R>::push(L, r);
        return r;
    }
};

static int luaWrapFunction(lua_State *L, function_none_to_none_t f) {
    expectNoArguments(L);
    f();
    return 0;
}

template<typename R>
static int luaWrapFunction(lua_State *L, function_none_to_one_t<R> f) {
    expectNoArguments(L);
    TypedStack<R>::push(L, f());
    return 1;
}

template<typename ...Args>
static int luaWrapFunction(lua_State *L, function_many_to_none_t<Args...> f) {
    expectArguments<Args...>(L);
    LuaCFunctionInvoker<void(Args...)>::invoke(L, f);
    return 0;
}

template<typename R, typename ...Args>
static int luaWrapFunction(lua_State *L, function_many_to_one_t<R, Args...> f) {
    expectArguments<Args...>(L);
    TypedStack<R>::push(L, LuaCFunctionInvoker<R(Args...)>::invoke(L, f));
    return 1;
}

template<typename ...Rs, typename ...Args>
static int luaWrapFunction(lua_State *L, function_many_to_one_t<std::tuple<Rs...>, Args...> f) {
    expectArguments<Args...>(L);
    TypedStack<Rs...>::push(L, LuaCFunctionInvoker<std::tuple<Rs...>(Args...)>::invoke(L, f));
    return sizeof...(Rs);
}

template<typename C>
static int luaWrapFunction(lua_State *L, class_none_to_none_t<C> f) {
    expectArguments<C *>(L);
    C *klass = getClass<C>(L);
    (klass->*f)();
    return 0;
}

template<typename C, typename R>
static int luaWrapFunction(lua_State *L, class_none_to_one_t<C, R> f) {
    expectArguments<C *>(L);
    C *klass = getClass<C>(L);
    TypedStack<R>::push(L, (klass->*f)());
    return 1;
}

template<typename C, typename ...Args>
static int luaWrapFunction(lua_State *L, class_many_to_none_t<C, Args...> f) {
    expectArguments<C *, Args...>(L);
    C *klass = getClass<C>(L);
    LuaCMethodInvoker<C(void(Args...))>::invoke(L, klass, f);
    return 0;
}

template<typename C, typename R, typename ...Args>
static int luaWrapFunction(lua_State *L, class_many_to_one_t<C, R, Args...> f) {
    expectArguments<C *, Args...>(L);
    C *klass = getClass<C>(L);
    TypedStack<R>::push(L, LuaCMethodInvoker<C(R(Args...))>::invoke(L, klass, f));
    return 1;
}

template<class C, typename T>
static int luaWrapPropertySet(lua_State *L, C *klass, class_property_t<C, T> p) {
    klass->*p = TypedStack<T>::pop(L);
    return 0;
}

template<class C, typename T>
static int luaWrapPropertyGet(lua_State *L, C *klass, class_property_t<C, T> p) {
    TypedStack<T>::push(L, klass->*p);
    return 1;
}

template<typename C, typename T>
static int luaWrapProperty(lua_State *L, class_property_t<C, T> p) {
    C *klass = getClass<C>(L);
    return lua_gettop(L) ? luaWrapPropertySet(L, klass, p) : luaWrapPropertyGet(L, klass, p);
}

template<typename C>
static int luaWrapMethod(lua_State *L, class_none_to_none_t<C> f) {
    expectNoArguments(L);
    C *klass = getClassByUpValue<C>(L);
    (klass->*f)();
    return 0;
}

template<typename C, typename R>
static int luaWrapMethod(lua_State *L, class_none_to_one_t<C, R> f) {
    expectNoArguments(L);
    C *klass = getClassByUpValue<C>(L);
    TypedStack<R>::push(L, (klass->*f)());
    return 1;
}

template<typename C, typename ...Args>
static int luaWrapMethod(lua_State *L, class_many_to_none_t<C, Args...> f) {
    expectArguments<Args...>(L);
    C *klass = getClassByUpValue<C>(L);
    LuaCMethodInvoker<C(void(Args...))>::invoke(L, klass, f);
    return 0;
}

template<typename C, typename R, typename ...Args>
static int luaWrapMethod(lua_State *L, class_many_to_one_t<C, R, Args...> f) {
    expectArguments<Args...>(L);
    C *klass = getClassByUpValue<C>(L);
    TypedStack<R>::push(L, LuaCMethodInvoker<C(R(Args...))>::invoke(L, klass, f));
    return 1;
}

#define mk_function(f) ([](lua_State *L) -> int { return luaWrapFunction(L, f); })
#define mk_property(p) ([](lua_State *L) -> int { return luaWrapProperty(L, p); })
#define mk_method(f) ([](lua_State *L) -> int { return luaWrapMethod(L, f); })

#endif //SIMPLELUA_WRAPPER_H
