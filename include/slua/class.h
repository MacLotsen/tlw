//
// Created by enijenhuis on 12-12-2019.
//

#ifndef SIMPLELUA_CLASS_H
#define SIMPLELUA_CLASS_H

//#include "stack.hpp"
#include "function.hpp"

template<typename C, typename R, typename ...ARGS>
using method_t = R (C::*)(ARGS...);

template<typename C, typename R>
using getter_t = R (C::*)();

template<typename C>
using call_t = void (C::*)();

template <typename C, typename ...ARGS>
using setter_t = void (C::*)(ARGS...);

// Keep the class methods in light userdata?

struct Prototype {
    int metatable = 0;
    lua_CFunction constructor = nullptr;
    lua_CFunction getter = nullptr;
    lua_CFunction setter = nullptr;
};

template<typename C>
int luaSafeMethodWrapper(lua_State *L, call_t<C> f) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i)
        std::cout << i << luaL_typename(L, i) << std::endl;
    C *klass = *((C**) lua_touserdata(L, 1));
    (klass->*f)();
    return 0;
}


template<typename C, typename ...ARGS>
int luaSafeMethodWrapper(lua_State *L, setter_t<C, ARGS...> f) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i)
        std::cout << i << luaL_typename(L, i) << std::endl;
    C *klass = *((C**) lua_touserdata(L, lua_upvalueindex(1)));
    StackInspector inspector(L);
    if (!inspector.expect<ARGS...>()) {
        errorUnmatchedArguments<ARGS...>(L);
    }
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    std::reverse(l.begin(), l.end());
    (klass->*f)((popArgument<ARGS>(l))...);
    for (AbstractValue *val: l)
        delete val;
    return 0;
}

template<typename C, typename R>
int luaSafeMethodWrapper(lua_State *L, getter_t<C, R> f) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i)
        std::cout << i << luaL_typename(L, i) << std::endl;
    C *klass = *((C**) lua_touserdata(L, 1));
    lua_remove(L, 1);
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    std::reverse(l.begin(), l.end());
    R result = (klass->*f)();
    for (AbstractValue *val: l)
        delete val;
    Value<R> resultValue(result);
    StackInserter inserter(L);
    inserter.insert(resultValue);
    return 1;
}

template<typename C, typename R, typename ...ARGS>
int luaSafeMethodWrapper(lua_State *L, method_t<C, R, ARGS...> f) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i)
        std::cout << i << luaL_typename(L, i) << std::endl;
    C *klass = *((C**) lua_touserdata(L, 1));
    lua_remove(L, 1);
    StackInspector inspector(L);
    if (!inspector.expect<ARGS...>()) {
        errorUnmatchedArguments<ARGS...>(L);
    }
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    std::reverse(l.begin(), l.end());
    R result = (klass->*f)((popArgument<ARGS>(l))...);
    for (AbstractValue *val: l)
        delete val;
    Value<R> resultValue(result);
    StackInserter inserter(L);
    inserter.insert(resultValue);
    return 1;
}

#define mk_method(m) ([](lua_State *L) -> int { return luaSafeMethodWrapper(L, m); })

#endif //SIMPLELUA_CLASS_H
