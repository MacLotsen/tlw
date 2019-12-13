//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_METHOD_H
#define SIMPLELUA_METHOD_H
#include "class.h"

template<typename C>
int luaSafeMethodWrapper(lua_State *L, call_t<C> f) {
    C *klass = *((C **) lua_touserdata(L, lua_upvalueindex(1)));
    (klass->*f)();
    return 0;
}


template<typename C, typename ...ARGS>
int luaSafeMethodWrapper(lua_State *L, setter_t<C, ARGS...> f) {
    C *klass = *((C **) lua_touserdata(L, lua_upvalueindex(1)));
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
    C *klass = *((C **) lua_touserdata(L, lua_upvalueindex(1)));
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
    C *klass = *((C **) lua_touserdata(L, lua_upvalueindex(1)));
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

#endif //SIMPLELUA_METHOD_H
