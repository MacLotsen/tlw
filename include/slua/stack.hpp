//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_STACK_HPP
#define SIMPLELUA_STACK_HPP

#include <lua.hpp>
#include <stack>
#include "metatable.h"

#ifndef luaL_setmetatable
#define luaL_setmetatable(_l, n) (luaL_getmetatable(_l, n), lua_setmetatable(_l, -2))
#endif

static const void *null_ref = nullptr;


class LuaStack {
private:
    lua_State *L;
public:
    explicit LuaStack(lua_State *L) : L(L) {}

    bool expect() {
        return lua_gettop(L) == 0;
    }

    template<typename LAST>
    bool expect(int index) {
        return !index || typeMatches<LAST>(index);
    }

    template<typename FIRST, typename SECOND, typename ...REST>
    bool expect(int index) {
        return !index || (typeMatches<FIRST>(index) && expect<SECOND, REST...>(index + 1));
    }

    template<typename ...ARGS>
    bool expect() {
        if (sizeof...(ARGS) != lua_gettop(L)) {
            std::cerr << "Expected " << sizeof...(ARGS) << " arguments. " << lua_gettop(L) << " were given."
                      << std::endl;
            return false;
        }
        return !sizeof...(ARGS) || expect<ARGS...>(1);
    }

    template<typename T>
    bool typeMatches(int index) {
        const std::type_info &type = typeid(T);
        if (type == typeid(Boolean)) {
            return lua_isboolean(L, index);
        } else if (type == typeid(Number)) {
            return lua_isnumber(L, index);
        } else if (type == typeid(String)) {
            return lua_isstring(L, index);
        } else if (type == typeid(List)) {
            return lua_istable(L, index);
        } else if (type == typeid(Table)) {
            return lua_istable(L, index);
        } else {
            return lua_isuserdata(L, index) || lua_islightuserdata(L, index) || lua_isnil(L, index);
        }
    }

    AbstractValue *pop() {
        AbstractValue *val;
        int t = lua_type(L, -1);
        switch (t) {
            case LUA_TBOOLEAN:
                val = new Value<Boolean>(lua_toboolean(L, -1));
                break;
            case LUA_TNUMBER:
                val = new Value<Number>(lua_tonumber(L, -1));
                break;
            case LUA_TSTRING:
                val = new Value<String>(String(lua_tostring(L, -1)));
                break;
            case LUA_TTABLE: {
                Table t;
                List l;
                // The keys of the table should be distinct,
                // or else we cannot make a difference between lists and tables
                unsigned int typeMask = 0;
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    int keyType = lua_type(L, -2);
                    if ((typeMask |= (1u << keyType)) != (1u << keyType)) {
                        lua_pushstring(L, "Mixed keys in table");
                        lua_error(L);
                    }
                    if (keyType == LUA_TNUMBER) {
                        l.push_back(pop());
                    } else {
                        std::string key = lua_tostring(L, -2);
                        t[key] = pop();
                    }
                }
                if (typeMask & (1u << LUA_TNUMBER)) {
                    val = new Value<List>(l);
                } else {
                    val = new Value<Table>(t);
                }
                break;
            }
            case LUA_TLIGHTUSERDATA: {
                auto v = (Class) lua_touserdata(L, -1);
                val = new Value<Class>(v);
                break;
            }
            case LUA_TUSERDATA: {
                auto v = (Class *) lua_touserdata(L, -1);
                val = new Value<Class>(v);
                break;
            }
            default:
                val = new Value<Class>(&null_ref);
        }
        lua_pop(L, 1);
        return val;
    }

    template<typename T>
    Value<T> *pop() {
        Value<T> *val;
        const std::type_info &typeInfo = typeid(T);
        if (typeInfo == typeid(Boolean)) {
            if (!lua_isboolean(L, -1)) {
                lua_pushstring(L, ("Type mismatch. Expected a boolean type, but " + String(luaL_typename(L, -1)) +
                                   " was given.").c_str());
                lua_error(L);
                return nullptr;
            }
            val = (Value<T>*) new Value<Boolean>(lua_toboolean(L, -1));
        } else if (typeInfo == typeid(Number) || typeInfo == typeid(int) || typeInfo == typeid(float)) {
            if (!lua_isnumber(L, -1)) {
                lua_pushstring(L, ("Type mismatch. Expected a number type, but " + String(luaL_typename(L, -1)) +
                                   " was given.").c_str());
                lua_error(L);
                return nullptr;
            }
            val = (Value<T>*) new Value<Number>(lua_tonumber(L, -1));
        } else if (typeInfo == typeid(String)) {
            if (!lua_isstring(L, -1)) {
                lua_pushstring(L, ("Type mismatch. Expected a string type, but " + String(luaL_typename(L, -1)) +
                                   " was given.").c_str());
                lua_error(L);
                return nullptr;
            }
            String s = lua_tostring(L, -1);
            val = (Value<T>*) new Value<String>(s);
        } else if (typeInfo == typeid(List)) {
            if (!lua_istable(L, -1)) {
                lua_pushstring(L, ("Type mismatch. Expected a list type, but " + String(luaL_typename(L, -1)) +
                                   " was given.").c_str());
                lua_error(L);
                return nullptr;
            }
            List l;
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                if (!lua_isnumber(L, -1)) {
                    lua_pushstring(L, ("Type mismatch. Expected an index of number type, but " +
                                       String(luaL_typename(L, -1)) + " was given.").c_str());
                    lua_error(L);
                    return nullptr;
                }
                l.push_back(pop());
            }

            val = (Value<T>*) new Value<List>(l);
        } else if (typeInfo == typeid(Table)) {
            if (!lua_istable(L, -1)) {
                lua_pushstring(L, ("Type mismatch. Expected a list type, but " + String(luaL_typename(L, -1)) +
                                   " was given.").c_str());
                lua_error(L);
                return nullptr;
            }
            Table t;
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                if (!lua_isstring(L, -1)) {
                    lua_pushstring(L, ("Type mismatch. Expected an index of string type, but " +
                                       String(luaL_typename(L, -1)) + " was given.").c_str());
                    lua_error(L);
                    return nullptr;
                }
                String key = lua_tostring(L, -2);
                t[key] = pop();
            }

            val = (Value<T>*) new Value<Table>(t);
        } else {
            int type = lua_type(L, -1);
            switch (type) {
                case LUA_TUSERDATA: {
                    T *pointerToPointer = (T *) lua_touserdata(L, -1);
                    val = new Value<T>(pointerToPointer);
                    break;
                }
                case LUA_TLIGHTUSERDATA: {
                    T v = *((T*) lua_touserdata(L, -1));
                    val = new Value<T>(v);
                    break;
                }
                case LUA_TNIL:
                case LUA_TNONE:
                    val = nullptr;
                    break;
                default:
                    val = nullptr;
                    lua_pushstring(L, ("Type mismatch. Expected a class type, but " + String(luaL_typename(L, -1)) +
                                       " was given.").c_str());
                    lua_error(L);
            }
        }
        lua_pop(L, 1);
        return val;
    }

    template<typename T>
    void push(Value<T> &val) {
        T v = dynamic_cast<T>(val.val());
        if (v) {
            if (MetaTable::metatables.count(&typeid(T))) {
                T *ud = (T *) lua_newuserdata(L, sizeof(T *));
                *ud = v;
                luaL_setmetatable(L, MetaTable::metatables[&typeid(T)].c_str());
            } else {
                lua_pushlightuserdata(L, (void *) v);
            }
        } else {
            lua_pushnil(L);
        }
    }

    void push(Value<Boolean> &val) {
        lua_pushboolean(L, val.val());
    }

    void push(Value<Number> &val) {
        lua_pushnumber(L, val.val());
    }

    void push(Value<int> &val) {
        lua_pushnumber(L, val.val());
    }

    void push(Value<String> &val) {
        lua_pushstring(L, val.val().c_str());
    }

    void push(Value<List> &val) {
        List l = val.val();
        lua_createtable(L, 0, l.size());
        for (int i = 0; i < l.size(); ++i) {
            lua_pushnumber(L, i + 1);
            push(*l.at(i));
            lua_settable(L, -3);
        }
    }

    void push(Value<Table> &val) {
        for (const auto &kv: val.val()) {
            push(*kv.second);
            lua_setfield(L, -2, kv.first.c_str());
        }
    }

    void push(AbstractValue &val) {
        if (val.is<Boolean>()) {
            lua_pushboolean(L, val.to<Boolean>());
        } else if (val.is<Number>() || val.is<int>()) {
            lua_pushnumber(L, val.to<Number>());
        } else if (val.is<String>()) {
            lua_pushstring(L, val.to<String>().c_str());
        } else if (val.is<List>()) {
            List l = val.to<List>();
            lua_createtable(L, 0, l.size());
            for (int i = 0; i < l.size(); ++i) {
                lua_pushnumber(L, i + 1);
                push(*l.at(i));
                lua_settable(L, -3);
            }
        } else if (val.is<Table>()) {
            for (const auto &kv: val.to<Table>()) {
                push(*kv.second);
                lua_setfield(L, -2, kv.first.c_str());
            }
        } else {
            Class v = val.to<Class>();
            if (v) {
                if (MetaTable::metatables.count(&val.getType())) {
                    Class *ud = (Class *) lua_newuserdata(L, sizeof(Class *));
                    *ud = v;
                    luaL_setmetatable(L, MetaTable::metatables[&val.getType()].c_str());
                } else {
                    lua_pushlightuserdata(L, (void *) v);
                }
            } else {
                lua_pushnil(L);
            }
        }
    }

    std::stack<AbstractValue *> getArgs() {
        std::stack<AbstractValue *> args;
        while (lua_gettop(L)) {
            args.push(pop());
        }
        return args;
    }
};

#endif //SIMPLELUA_STACK_HPP
