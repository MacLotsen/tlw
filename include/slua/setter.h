//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_SETTER_H
#define SIMPLELUA_SETTER_H

#include "class.h"

template<typename C, typename ...ARGS>
int lauSafeSetterWrapper(lua_State *L, setter_t<C, ARGS...> f) {
    C *klass = *((C **) lua_touserdata(L, 1));
    lua_remove(L, 1);
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

template<typename C, typename R, typename ...ARGS>
int lauSafeSetterWrapper(lua_State *L, method_t<C, R, ARGS...> f) {
    C *klass = *((C **) lua_touserdata(L, 1));
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

#define mk_setter(m) ([](lua_State *L) -> int { return lauSafeSetterWrapper(L, m); })

#endif //SIMPLELUA_SETTER_H
