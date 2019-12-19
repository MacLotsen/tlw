//
// Created by enijenhuis on 16-12-2019.
//

#ifndef SIMPLELUA_WRAPPER_H
#define SIMPLELUA_WRAPPER_H

#include <functional>

using function_none_to_none_t = void (*)();

template<typename R>
using function_none_to_one_t = R (*)();

template<typename ...ARGS>
using function_many_to_none_t = void (*)(ARGS...);

template<typename R, typename ...ARGS>
using function_many_to_one_t = R (*)(ARGS...);

template<class C, typename T>
using class_property_t = T C::*;

template<typename C>
using class_none_to_none_t = void (C::*)();

template<typename C, typename R>
using class_none_to_one_t = R (C::*)();

template<typename C, typename ...ARGS>
using class_many_to_none_t = void (C::*)(ARGS...);

template<typename C, typename R, typename ...ARGS>
using class_many_to_one_t = R (C::*)(ARGS...);

template<typename LAST>
static std::string typesToString() {
    return typeid(LAST).name();
}

template<typename FIRST, typename SECOND, typename ...ARGS>
static std::string typesToString() {
    return std::string(typeid(FIRST).name()) + ", " + typesToString<SECOND, ARGS...>();
}

template<typename ...ARGS>
static void errorUnmatchedArguments(lua_State *L) {
    std::string errorMessage = "Expected: " + typesToString<ARGS...>();
    lua_pushstring(L, errorMessage.c_str());
    lua_error(L);
}

static void expectNoArguments(lua_State *L) {
    if (lua_gettop(L)) {
        lua_pushstring(L, "Expected no arguments.");
        lua_error(L);
    }
}

template<typename ...ARGS>
static void expectArguments(lua_State *L) {
    if (!LuaStack(L).expect<ARGS...>()) {
        errorUnmatchedArguments<ARGS...>(L);
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
static T getArg(std::stack<AbstractValue *> &stack) {
    AbstractValue *value = stack.top();
    T ret = value->to<T>();
    delete value;
    stack.pop();
    return ret;
}

static int luaWrapFunction(lua_State *L, function_none_to_none_t f) {
    expectNoArguments(L);
    f();
    return 0;
}

template<typename R>
static int luaWrapFunction(lua_State *L, function_none_to_one_t<R> f) {
    expectNoArguments(L);
    R r = f();
    LuaStack stack(L);
    Value<R> v(r);
    stack.push(v);
    return 1;
}

template<typename ...ARGS>
static int luaWrapFunction(lua_State *L, function_many_to_none_t<ARGS...> f) {
    expectArguments<ARGS...>(L);
    LuaStack stack(L);
    auto s = stack.getArgs();
    f(getArg<ARGS>(s)...);
    return 0;
}

template<typename R, typename ...ARGS>
static int luaWrapFunction(lua_State *L, function_many_to_one_t<R, ARGS...> f) {
    expectArguments<ARGS...>(L);
    LuaStack stack(L);
    auto s = stack.getArgs();
    Value<R> r(f(getArg<ARGS>(s)...));
    stack.push(r);
    return 1;
}

template<typename C>
static int luaWrapFunction(lua_State *L, class_none_to_none_t<C> f) {
    expectArguments<C>(L);
    LuaStack stack(L);
    Value<C *> *klassValue = stack.pop<C *>();
    C *klass = klassValue->val();
    delete klassValue;
    (klass->*f)();
    return 0;
}

template<typename C, typename R>
static int luaWrapFunction(lua_State *L, class_none_to_one_t<C, R> f) {
    expectArguments<C *>(L);
    LuaStack stack(L);
    Value<C *> *klassValue = stack.pop<C *>();
    C *klass = klassValue->val();
    delete klassValue;
    Value<R> r((klass->*f)());
    stack.push(r);
    return 1;
}

template<typename C, typename ...ARGS>
static int luaWrapFunction(lua_State *L, class_many_to_none_t<C, ARGS...> f) {
    expectArguments<C *, ARGS...>(L);
    LuaStack stack(L);
    C *klass = getClass<C>(L);
    auto s = stack.getArgs();
    (klass->*f)(getArg<ARGS>(s)...);
    return 0;
}

template<typename C, typename R, typename ...ARGS>
static int luaWrapFunction(lua_State *L, class_many_to_one_t<C, R, ARGS...> f) {
    expectArguments<C *, ARGS...>(L);
    LuaStack stack(L);
    C *klass = getClass<C>(L);
    Value<R> r((klass->*f)(getArg<ARGS>(stack.getArgs())...));
    stack.push(r);
    return 1;
}

template<class C, typename T>
static int luaWrapPropertySet(lua_State *L, C *klass, class_property_t<C, T> p) {
    LuaStack stack(L);
    Value<T> *val = stack.pop<T>();
    klass->*p = val->val();
    delete val;
    return 0;
}

template<class C, typename T>
static int luaWrapPropertyGet(lua_State *L, C *klass, class_property_t<C, T> p) {
    LuaStack stack(L);
    T val = klass->*p;
    Value<T> v(&val);
    stack.push(v);
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
    LuaStack stack(L);
    Value<R> r((klass->*f)());
    stack.push(r);
    return 1;
}

template<typename C, typename ...ARGS>
static int luaWrapMethod(lua_State *L, class_many_to_none_t<C, ARGS...> f) {
    expectArguments<ARGS...>(L);
    LuaStack stack(L);
    C *klass = getClassByUpValue<C>(L);
    auto s = stack.getArgs();
    (klass->*f)(getArg<ARGS>(s)...);
    return 0;
}

template<typename C, typename R, typename ...ARGS>
static int luaWrapMethod(lua_State *L, class_many_to_one_t<C, R, ARGS...> f) {
    expectArguments<ARGS...>(L);
    LuaStack stack(L);
    C *klass = getClassByUpValue<C>(L);
    auto s = stack.getArgs();
    Value<R> r((klass->*f)(getArg<ARGS>(s)...));
    stack.push(r);
    return 1;
}


template<typename R, typename ...Args>
static int luaFunctionExecutioner(lua_State *L) {
    R (**f)(Args...) = (R (**)(Args...)) lua_touserdata(L, lua_upvalueindex(1));
    LuaStack stack(L);
    auto args = stack.getArgs();
    R r = (*f)(getArg<Args>(args)...);
    Value<R> ret(r);
    stack.push(ret);
    return 1;
}

template<typename R, typename ...Args>
void mkfunc(lua_State *L, R (*f)(Args...)) {
    R (**F)(Args...) = (R(**)(Args...)) lua_newuserdata(L, sizeof(R(**)(Args...)));
    *F = f;
    lua_pushcclosure(L, luaFunctionExecutioner<R, Args...>, 1);
}

#define mk_function(f) ([](lua_State *L) -> int { return luaWrapFunction(L, f); })
#define mk_property(p) ([](lua_State *L) -> int { return luaWrapProperty(L, p); })
#define mk_method(f) ([](lua_State *L) -> int { return luaWrapMethod(L, f); })

#endif //SIMPLELUA_WRAPPER_H
