//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_PROPERTY_H
#define SIMPLELUA_PROPERTY_H

#include "class.h"

template<class C, typename T>
using prop_t = T C::*;

template<class C, typename T>
int luaPropertySetWrapper(lua_State *L, C *klass, prop_t<C, T> p) {
    StackFetcher fetcher(L);
    klass->*p = fetcher.get<T>(-1)->val();
    return 0;
}

template<class C, typename T>
int luaPropertyGetWrapper(lua_State *L, C *klass, prop_t<C, T> p) {
    StackInserter inserter(L);
    T val = klass->*p;
    Value<T> v(&val);
    inserter.insert(v);
    return 1;
}

template<class C, typename T>
int luaSafePropertyWrapper(lua_State *L, prop_t<C, T> p) {
    C *klass = *((C **) lua_touserdata(L, 1));
    lua_remove(L, 1);
    return lua_gettop(L) ? luaPropertySetWrapper(L, klass, p) : luaPropertyGetWrapper(L, klass, p);
}

#define mk_prop(p) ([](lua_State *L) -> int { return luaSafePropertyWrapper(L, p); })

#endif //SIMPLELUA_PROPERTY_H
