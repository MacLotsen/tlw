//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_FUNCTION_HPP
#define SIMPLELUA_FUNCTION_HPP

#include <algorithm>
#include "types.hpp"
#include "stack.hpp"

template<typename R, typename ...ARGS>
using func_t = R (*)(ARGS...);

template<typename ...ARGS>
using func_void_t = void (*)(ARGS...);

template<typename R>
using func_get_t = R (*)();

template<typename LAST>
std::string typesToString() {
    return typeid(LAST).name();
}

template<typename FIRST, typename SECOND, typename ...ARGS>
std::string typesToString() {
    return std::string(typeid(FIRST).name()) + ", " + typesToString<SECOND, ARGS...>();
}

template<typename ...ARGS>
void errorUnmatchedArguments(lua_State *L) {
    std::string errorMessage = "Expected: " + typesToString<ARGS...>();
    lua_pushstring(L, errorMessage.c_str());
    lua_error(L);
}

template<typename T>
T popArgument(List &list) {
    T val = list.back()->to<T>();
    list.pop_back();
    return val;
}

static int luaFunctionWrapper(lua_State *L, func_t<List, List> f) {
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    List result = f(l);
    for (AbstractValue *v: l)
        delete v;
    StackInserter inserter(L);
    for (AbstractValue *val: result)
        inserter.insert(*val);
    for (AbstractValue *val: result)
        delete val;
    return result.size();
}


static int luaSafeFunctionWrapper(lua_State *L, void (*f)()) {
    f();
    return 0;
}

template<typename ...ARGS>
int luaSafeFunctionWrapper(lua_State *L, func_void_t <ARGS...> f) {
    StackInspector inspector(L);
    if (!inspector.expect<ARGS...>()) {
        errorUnmatchedArguments<ARGS...>(L);
    }
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    std::reverse(l.begin(), l.end());
    f((popArgument<ARGS>(l))...);
    return 0;
}

template<typename R>
int luaSafeFunctionWrapper(lua_State *L, func_get_t <R> f) {
    int argc = lua_gettop(L);
    if (argc) {
        lua_pushstring(L, ("Zero arguments expected. " + std::to_string(argc) + " give.").c_str());
        lua_error(L);
    }
    R result = f();
    Value<R> resultValue(result);
    StackInserter inserter(L);
    inserter.insert(resultValue);
    return 1;
}

template<typename R, typename ...ARGS>
int luaSafeFunctionWrapper(lua_State *L, func_t<R, ARGS...> f) {
    StackInspector inspector(L);
    if (!inspector.expect<ARGS...>()) {
        errorUnmatchedArguments<ARGS...>(L);
    }
    StackFetcher fetcher(L);
    Value<List> *list = fetcher.get();
    List l = list->val();
    std::reverse(l.begin(), l.end());
    R result = f((popArgument<ARGS>(l))...);
    for (AbstractValue *val: l)
        delete val;
    Value<R> resultValue(result);
    StackInserter inserter(L);
    inserter.insert(resultValue);
    return 1;
}

#define mk_cfunc(f) ([](lua_State *L) -> int { return luaSafeFunctionWrapper(L, f); })
#define mk_raw_cfunc(f) ([](lua_State *L) -> int { return luaFunctionWrapper(L, f); })

#endif //SIMPLELUA_FUNCTION_HPP
