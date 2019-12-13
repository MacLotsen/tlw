//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_GETTER_H
#define SIMPLELUA_GETTER_H

#include "class.h"

template<typename C, typename R>
int lauSafeGetterWrapper(lua_State *L, getter_t<C, R> f) {
    C *klass = *((C **) lua_touserdata(L, 1));
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

#define mk_getter(m) ([](lua_State *L) -> int { return lauSafeGetterWrapper(L, m); })

#endif //SIMPLELUA_GETTER_H
