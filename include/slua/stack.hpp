//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_STACK_HPP
#define SIMPLELUA_STACK_HPP


class StackInspector {
private:
    lua_State *L;
public:
    explicit StackInspector(lua_State *L) : L(L) {}

    bool expect() {
        return lua_gettop(L) == 0;
    }

    template<typename ...ARGS>
    bool expect() {
        if (sizeof...(ARGS) != lua_gettop(L)) {
            std::cerr << "Expected " << sizeof...(ARGS) << " arguments. " << lua_gettop(L) << " were given."
                      << std::endl;
            return false;
        }
        return !sizeof...(ARGS) || _expect<ARGS...>(1);
    }

private:

    template<typename LAST>
    bool _expect(int index) {
        return !index || typeMatches<LAST>(index);
    }

    template<typename FIRST, typename SECOND, typename ...REST>
    bool _expect(int index) {
        return !index || (typeMatches<FIRST>(index) && _expect<SECOND, REST...>(index + 1));
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
        } else if (type == typeid(Class)) {
            return lua_isuserdata(L, index) || lua_isnil(L, index);
        }
        std::cerr << "Unknown type given: " << type.name() << std::endl;
        return false;
    }
};

class StackFetcher {
private:
    lua_State *L;
public:
    explicit StackFetcher(lua_State *L) : L(L) {}

    Value<List> *get() {
        List l;
        int n = lua_gettop(L);
        for (int i = 1; i <= n; ++i) {
            l.push_back(get(i));
        }
        return new Value<List>(l);
    }

    template<typename T>
    Value<T> *get(int index) {
        auto value = static_cast<Value<T> *>(get(index));
        return value;
    }

    AbstractValue *get(int index) {
        AbstractValue *value = nullptr;
        if (lua_isboolean(L, index)) {
            value = new Value<Boolean>(lua_toboolean(L, index));
        } else if (lua_type(L, index) == LUA_TNUMBER) {
            value = new Value<Number>(lua_tonumber(L, index));
        } else if (lua_isstring(L, index)) {
            value = new Value<String>(String(lua_tostring(L, index)));
        } else if (lua_istable(L, index)) {
            Table table;
            List list;
            int isTableOrList = 0;
//            lua_gettable(L, index);
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                if (!isTableOrList) {
                    if (lua_isnumber(L, -2)) {
                        isTableOrList = 1;
                    } else {
                        isTableOrList = -1;
                    }
                }
                if (isTableOrList == 1) {
                    list.push_back(get(-1));
                } else {
                    std::string key = lua_tostring(L, -2);
                    table[key] = get(-1);
                }
                lua_pop(L, 1);
            }
//            lua_pop(L, 1);
            if (isTableOrList == 1) {
                value = new Value<List>(list);
            } else {
                value = new Value<Table>(table);
            }
        } else if (lua_isuserdata(L, index)) {
            value = new Value<Class>(lua_touserdata(L, index));
        }
        return value;
    }

    void dump() {
        int n = lua_gettop(L);
        for (int i = 1; i <= n; ++i) {
            std::string type = luaL_typename(L, i);
            std::cout << "Index: " << i << " Type: " << type << std::endl;
        }
    }
};

class StackInserter {
private:
    lua_State *L;
public:
    explicit StackInserter(lua_State *L) : L(L) {}

    template<typename T>
    void insert(Value<T> &val) {
        insert((AbstractValue &) val);
    }

    void insert(AbstractValue &val) {
        if (val.is<Boolean>()) {
            lua_pushboolean(L, val.to<Boolean>());
        } else if (val.is<Number>()) {
            lua_pushnumber(L, val.to<Number>());
        } else if (val.is<String>()) {
            lua_pushstring(L, val.to<String>().c_str());
        } else if (val.is<List>()) {
            List l = val.to<List>();
            lua_createtable(L, 0, l.size());
            for (int i = 0; i < l.size(); ++i) {
                lua_pushnumber(L, i + 1);
                insert(*l.at(i));
                lua_settable(L, -3);
            }
        } else if (val.is<Table>()) {
            for (const auto &kv: val.to<Table>()) {
                insert(*kv.second);
                lua_setfield(L, -2, kv.first.c_str());
            }
        } else if (val.is<Class>()) {
            // TODO: implement class inserter
        } else {
            lua_pushnil(L);
        }
    }
};

#endif //SIMPLELUA_STACK_HPP
