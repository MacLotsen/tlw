//
// Created by enijenhuis on 17-12-2019.
//
#include <gtest/gtest.h>
#include <lua.hpp>
#include <functional>
#include <variant>

static std::unordered_map<const std::type_info *, const std::string &> CLASS_NAMES;

template<typename ...T>
class TypedStack {

};

/**
 * Unknown types will be used for user data
 * @tparam T
 */
template<typename T>
class TypedStack<T> {
public:
    inline static T &get(lua_State *L, int i) {
        if (lua_islightuserdata(L, i)) {
            return *((T *) lua_touserdata(L, i));
        } else {
            std::string errorMsg = "Expected a class/struct argument. " + std::string(luaL_typename(L, i)) + " given.";
            throw std::runtime_error(errorMsg);
        }
    }

    inline static T &pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, T *o) {
        lua_pushlightuserdata(L, (void *) o);
    }
};

template<typename T>
class TypedStack<T *> {
public:
    inline static T *get(lua_State *L, int i) {
        if (CLASS_NAMES.count(&typeid(T *))) {
            T **ptp = (T **) lua_touserdata(L, i);
            return *ptp;
        } else {
            auto p = (T *) lua_touserdata(L, i);
            return p;
        }
    }

    inline static T *pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, T *o) {
        if (CLASS_NAMES.count(&typeid(T *))) {
            T **userDatum = lua_newuserdata(L, sizeof(T **));
            *userDatum = o;
            luaL_getmetatable(L, CLASS_NAMES[&typeid(T *)].c_str());
            lua_setmetatable(L, -2);
        } else {
            lua_pushlightuserdata(L, (void *) o);
        }
    }
};

template<typename T>
class TypedStack<const T> {
};

template<typename T>
class TypedStack<T &> {
};

template<typename T>
class TypedStack<const T &> {
};

template<typename T>
class TypedStack<volatile T &> {
    inline static void push(lua_State *L, volatile T &o) {
        TypedStack<T>::push(L, const_cast<const T &>(o));
    }
};

template<typename T>
class TypedStack<const volatile T &> {
    inline static void push(lua_State *L, const volatile T &o) {
        TypedStack<T>::push(L, const_cast<const T &>(o));
    }
};

template<typename T>
class TypedStack<T &&> {
    inline static void push(lua_State *L, T &&o) {
        TypedStack<T>::push(L, std::forward<T>(o));
    }
};

template<>
class TypedStack<bool> {
public:
    inline static bool get(lua_State *L, int i) {
        return lua_toboolean(L, i);
    }

    inline static bool pop(lua_State *L) {
        auto b = get(L, -1);
        lua_pop(L, 1);
        return b;
    }

    inline static void push(lua_State *L, const bool &o) {
        lua_pushboolean(L, o);
    }
};

template<>
class TypedStack<const bool &> {
public:
    inline static void push(lua_State *L, const bool &o) {
        lua_pushboolean(L, o);
    }
};

template<>
class TypedStack<int> {
public:
    inline static int get(lua_State *L, int i) {
        if (lua_isnumber(L, i)) {
            return int(lua_tointeger(L, i));
        }
        lua_pushstring(L, "NaN");
        lua_error(L);
        return 0;
    }

    inline static int pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, int n) {
        lua_pushinteger(L, n);
    }
};

template<>
class TypedStack<const int &> {
public:
    inline static void push(lua_State *L, const int &n) {
        lua_pushinteger(L, n);
    }
};

template<>
class TypedStack<float> {
public:
    inline static float get(lua_State *L, int i) {
        if (lua_isnumber(L, i)) {
            return float(lua_tonumber(L, i));
        }
        lua_pushstring(L, "NaN");
        lua_error(L);
        return 0;
    }

    inline static float pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, float n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const float &> {
public:
    inline static void push(lua_State *L, const float &n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<double> {
public:
    inline static double get(lua_State *L, int i) {
        if (lua_isnumber(L, i)) {
            return lua_tonumber(L, i);
        }
        lua_pushstring(L, "NaN");
        lua_error(L);
        return 0;
    }

    inline static double pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, double n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const double &> {
public:
    inline static void push(lua_State *L, const double &n) {
        lua_pushnumber(L, n);
    }
};

template<>
class TypedStack<const char *> {
public:
    inline static const char *get(lua_State *L, int i) {
        return lua_tostring(L, i);
    }

    inline static const char *pop(lua_State *L) {
        auto r = get(L, -1);
        lua_pop(L, 1);
        return r;
    }

    inline static void push(lua_State *L, const char *o) {
        lua_pushstring(L, o);
    }
};

template<>
class TypedStack<char *> {
public:
    inline static char *get(lua_State *L, int i) {
        return const_cast<char *>(TypedStack<const char *>::get(L, i));
    }

    inline static char *pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, const char *o) {
        TypedStack<const char *>::push(L, o);
    }
};

template<>
class TypedStack<std::string> {
public:
    inline static std::string get(lua_State *L, int i) {
        return TypedStack<const char *>::get(L, i);
    }

    inline static std::string pop(lua_State *L) {
        auto value = get(L, -1);
        lua_pop(L, 1);
        return value;
    }

    inline static void push(lua_State *L, const std::string &o) {
        TypedStack<const char *>::push(L, o.c_str());
    }
};

template<>
class TypedStack<const std::string &> {
public:
    inline static void push(lua_State *L, const std::string &o) {
        TypedStack<const char *>::push(L, o.c_str());
    }
};

template<typename ...Ts>
class TypedStack<std::variant<Ts...>> {
    using T = std::variant<Ts...>;
public:
    inline static T get(lua_State *L, int i) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TBOOLEAN:
                return TypedStack<bool>::get(L, i);
            case LUA_TNUMBER:
                return TypedStack<double>::get(L, i);
            case LUA_TSTRING:
                if constexpr ((std::is_same_v<std::string, Ts> || ...)) {
                    return TypedStack<std::string>::get(L, i);
                } else {
                    return TypedStack<const char *>::get(L, i);
                }
            case LUA_TTABLE:
                // TODO
                break;
            case LUA_TLIGHTUSERDATA:
                // TODO
                break;
            case LUA_TUSERDATA:
                // TODO
                break;
        }
    }

    inline static T pop(lua_State *L) {
        T v = get(L, -1);
        lua_pop(L, 1);
        return v;
    }

    inline static void push(lua_State *L, const T &v) {
        push<Ts...>(L, v);
    }

    template<typename Arg>
    inline static void push(lua_State *L, const T &v) {
        if (std::holds_alternative<Arg>(v)) {
            TypedStack<Arg>::push(L, std::get<Arg>(v));
        } else {
            throw std::runtime_error("Incompatible variant type given.");
        }
    }

    template<typename Arg1, typename Arg2, typename ...Args>
    inline static void push(lua_State *L, const T &v) {
        if (std::holds_alternative<Arg1>(v)) {
            TypedStack<Arg1>::push(L, std::get<Arg1>(v));
        } else {
            push<Arg2, Args...>(L, v);
        }
    }
};

template<typename K, typename ...Args>
class TypedStack<std::vector<K, Args...>> {
    typedef std::vector<K, Args...> T;
public:
    inline static T get(lua_State *L, int i) {
        T v;
        lua_pushnil(L);
        while (lua_next(L, i > 0 ? i : i - 1)) {
            v.push_back(TypedStack<typename T::value_type>::pop(L));
        }
        return v;
    }

    inline static T pop(lua_State *L) {
        T v = get(L, -1);
        lua_pop(L, 1);
        return v;
    }

    inline static void push(lua_State *L, const T &v) {
        lua_newtable(L);
        int i = 1;
        for (auto it = v.begin(); it != v.end(); ++it) {
            TypedStack<typename T::value_type>::push(L, *it);
            lua_rawseti(L, -2, i++);
        }
    }
};

template<typename K, typename V, typename ...Args>
class TypedStack<std::unordered_map<K, V, Args...>> {
    using T = std::unordered_map<K, V, Args...>;
public:
    /* TODO single value typed map */
};

template<typename ...Args>
class TypedStack<std::unordered_map<const std::string &, std::variant<Args...>>> {
    using T = std::unordered_map<const std::string &, std::variant<Args...>>;
public:
    /* TODO variant map */
};

template<int... Is>
struct seq {
};

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {
};

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> {
};

template<typename ...Args>
class TypedStack<std::tuple<Args...>> {
    typedef std::tuple<Args...> T;
public:
    inline static T get(lua_State *L, int i) {
        return get(L, i, gen_seq<sizeof...(Args)>());
    }

    template<int ...Is>
    inline static T get(lua_State *L, int i, seq<Is...>) {
        return {fetch<Args>(L, i, Is + 1)...};
    }

    template<typename E>
    inline static E fetch(lua_State *L, int tableIndex, int index) {
        lua_rawgeti(L, tableIndex, index);
        return TypedStack<E>::pop(L);
    }

    inline static T pop(lua_State *L) {
        T values = get(L, -1);
        lua_pop(L, 1);
        return values;
    }

    inline static void push(lua_State *L, const T &t) {
        lua_newtable(L);
        push(L, t, gen_seq<sizeof...(Args)>());
    }

    template<int ...Is>
    inline static void push(lua_State *L, const T &t, seq<Is...>) {
        (..., insert<Args>(L, std::get<Is>(t), -2, Is));
    }

    template<typename E>
    inline static void insert(lua_State *L, E e, int tableIndex, int index) {
        TypedStack<E>::push(L, e);
        lua_rawseti(L, tableIndex, index + 1);
    }
};

template<typename T1, typename T2, typename ...Ts>
class TypedStack<T1, T2, Ts...> {
public:
    inline static std::tuple<T1, T2, Ts...> pop(lua_State *L) {
        auto results = get(L);
        lua_settop(L, 0);
        return results;
    }

    inline static void push(lua_State *L, T1 o1, T2 o2, Ts...os) {
        TypedStack<T1>::push(L, o1);
        TypedStack<T2>::push(L, o2);
        (..., TypedStack<Ts>::push(L, os));
    }

private:
    inline static std::tuple<T1, T2, Ts...> get(lua_State *L) {
        return getAll(L, gen_seq<sizeof...(Ts)>());
    }

    template<int ...Is>
    inline static std::tuple<T1, T2, Ts...> getAll(lua_State *L, seq<Is...>) {
        return {TypedStack<T1>::get(L, 1), TypedStack<T2>::get(L, 2), TypedStack<Ts>::get(L, Is + 3)...};
    }
};

class LuaRef {
protected:
    lua_State *L;
    int index = -1;
public:
    LuaRef(lua_State *L) : L(L) {
        index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    ~LuaRef() {
        luaL_unref(L, LUA_REGISTRYINDEX, index);
    }
};

template<typename ...T>
class LuaFunction : public LuaRef {

};

template<>
class LuaFunction<> : public LuaRef {
public:
    void operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 0, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
    }
};

template<typename R, typename ...Args>
class LuaFunction<R(Args...)> : public LuaRef {
public:
    R operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 1, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<R>::pop(L);
    }
};

template<typename ...Rs, typename ...Args>
class LuaFunction<std::tuple<Rs...>(Args...)> : public LuaRef {
public:
    std::tuple<Rs...> operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), sizeof...(Rs), 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<Rs...>::pop(L);
    }
};

template<typename R>
class LuaFunction<R()> : public LuaRef {
public:
    R operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, 1, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<R>::pop(L);
    }
};

template<typename ...Rs>
class LuaFunction<std::tuple<Rs...>()> : public LuaRef {
public:
    std::tuple<Rs...> operator()() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        if (lua_pcall(L, 0, sizeof...(Rs), 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        return TypedStack<Rs...>::pop(L);
    }
};

template<typename ...Args>
class LuaFunction<void(Args...)> : public LuaRef {
public:
    void operator()(Args...args) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, index);
        (..., TypedStack<Args>::push(L, args));
        if (lua_pcall(L, sizeof...(Args), 0, 0)) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
    }
};


template<typename R, typename ...Args>
class TypedStack<LuaFunction<R(Args...)>> {
    typedef LuaFunction<R(Args...)> T;
public:
    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<typename ...Args>
class TypedStack<LuaFunction<void(Args...)>> {
    typedef LuaFunction<void(Args...)> T;
public:
    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<typename R>
class TypedStack<LuaFunction<R()>> {
    typedef LuaFunction<R()> T;
public:
    inline static T pop(lua_State *L) {
        return T{L};
    }
};

template<>
class TypedStack<LuaFunction<>> {
    typedef LuaFunction<> T;
public:
    inline static T pop(lua_State *L) {
        return T{L};
    }
};

class Stack {
private:
    lua_State *L;
public:
    explicit Stack(lua_State *L) : L(L) {}

    template<typename T>
    T pop() {
        return TypedStack<T>::pop(L);
    }

    template<typename Arg1, typename Arg2, typename ...Args>
    std::tuple<Arg1, Arg2, Args...> pop() {
        return TypedStack<Arg1, Arg2, Args...>::pop(L);
    }

    template<typename T>
    void push(T o) {
        TypedStack<T>::push(L, o);
    }

    template<typename Arg1, typename Arg2, typename ...Args>
    void push(Arg1 arg1, Arg2 arg2, Args... args) {
        TypedStack<Arg1, Arg2, Args...>::push(L, arg1, arg2, args...);
    }
};

TEST(StackTest, testValueTypes) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test boolean
    lua_pushboolean(L, true);
    ASSERT_TRUE(s.pop<bool>());

    ASSERT_EQ(0, lua_gettop(L));

    // Test Number
    lua_pushnumber(L, 2.5);
    auto myNumber = s.pop<double>();
    ASSERT_EQ(2.5, myNumber);

    ASSERT_EQ(0, lua_gettop(L));

    // Test C String
    lua_pushstring(L, "LUA");
    lua_pushvalue(L, -1);
    auto cstr = s.pop<const char *>();
    ASSERT_STREQ("LUA", cstr);

    ASSERT_EQ(1, lua_gettop(L));

    auto str = s.pop<std::string>();
    ASSERT_STREQ("LUA", str.c_str());

    ASSERT_EQ(0, lua_gettop(L));
    lua_close(L);
}

TEST(StackTest, testVariant) {
    using T = std::variant<bool, double, std::string, const char *>;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    T var1;
    lua_pushboolean(L, true);
    var1 = s.pop<T>();
    ASSERT_TRUE(std::holds_alternative<bool>(var1));
    ASSERT_TRUE(std::get<bool>(var1));

    T var2;
    lua_pushnumber(L, 2.5);
    var2 = s.pop<T>();
    ASSERT_TRUE(std::holds_alternative<double>(var2));
    ASSERT_EQ(2.5, std::get<double>(var2));

    T var3;
    lua_pushstring(L, "LUA");
    var3 = s.pop<T>();
    ASSERT_TRUE(std::holds_alternative<std::string>(var3));
    ASSERT_EQ("LUA", std::get<std::string>(var3));

    // Test pushing a variant
    T var4 = "LUA";
    ASSERT_FALSE(std::holds_alternative<bool>(var4));
    ASSERT_FALSE(std::holds_alternative<double>(var4));
    ASSERT_FALSE(std::holds_alternative<std::string>(var4));
    ASSERT_TRUE(std::holds_alternative<const char *>(var4));
    s.push(var4);
    ASSERT_EQ(1, lua_gettop(L));
    ASSERT_EQ("LUA", s.pop<std::string>());

    lua_close(L);
}

TEST(StackTest, testVector) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test vector
    std::vector<double> values = {1, 4, 9};
    s.push(values);

    auto l = s.pop<std::vector<double>>();
    ASSERT_EQ(3, l.size());

    ASSERT_EQ(1, l.at(0));
    ASSERT_EQ(4, l.at(1));
    ASSERT_EQ(9, l.at(2));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

TEST(StackTest, testTuple) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    // Test tuple
    s.push<std::tuple<bool, double, std::string>>({true, 2.5, "LUA"});
    ASSERT_EQ(1, lua_gettop(L));

    auto t = s.pop<std::tuple<bool, double, std::string>>();
    ASSERT_TRUE(std::get<0>(t));
    ASSERT_EQ(2.5, std::get<1>(t));
    ASSERT_EQ("LUA", std::get<2>(t));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

TEST(StackTest, testPopArgumentsAsTuple) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    s.push(true, 2.5, "LUA");
    ASSERT_EQ(3, lua_gettop(L));

    auto t = s.pop<bool, double, std::string>();
    ASSERT_TRUE(std::get<0>(t));
    ASSERT_EQ(2.5, std::get<1>(t));
    ASSERT_EQ("LUA", std::get<2>(t));

    ASSERT_EQ(0, lua_gettop(L));

    // Test with minimum types
    s.push(true, false);

    ASSERT_EQ(2, lua_gettop(L));

    auto t2 = s.pop<bool, bool>();
    ASSERT_TRUE(std::get<0>(t2));
    ASSERT_FALSE(std::get<1>(t2));

    ASSERT_EQ(0, lua_gettop(L));

    lua_close(L);
}

//TEST(StackTest, testTable) {
//    lua_State *L = luaL_newstate();
//    luaL_openlibs(L);
//    Stack s(L);
//
//    // Test table/struct
//    lua_newtable(L);
//    lua_pushnumber(L, 1.5);
//    lua_setfield(L, -2, "number");
//    lua_pushstring(L, "a string");
//    lua_setfield(L, -2, "str");
//
//    /*
//    auto t = s.pop<TableTest>();
//    ASSERT_EQ(1.5, t.number);
//    ASSERT_EQ("a string", t.str);
//     */
//    lua_close(L);
//}

TEST(StackTest, testLuaFunctions) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    luaL_dofile(L, "scripts/functions.lua");

    // Let the functions live within an active lua state (see last line)
    {
        lua_getglobal(L, "f1");
        auto f1 = s.pop<LuaFunction<>>();
        f1();
        lua_getglobal(L, "r1");
        ASSERT_TRUE(s.pop<bool>());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f2");
        auto f2 = s.pop<LuaFunction<bool()>>();
        ASSERT_TRUE(f2());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f3");
        auto f3 = s.pop<LuaFunction<void(std::string, std::string)>>();
        f3("John", "Doe");
        lua_getglobal(L, "r3");
        ASSERT_EQ("John Doe", s.pop<std::string>());

        ASSERT_EQ(0, lua_gettop(L));

        lua_getglobal(L, "f4");
        auto f4 = s.pop<LuaFunction<double(double, double)>>();
        ASSERT_EQ(5.0, f4(2.5, 2.5));

        ASSERT_EQ(0, lua_gettop(L));
    }

    lua_close(L);
}

TEST(StackTest, testScriptAsFunction) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    luaL_loadstring(L, "return true, 1.0, 'true'\n");
    {
        auto f = s.pop<LuaFunction<std::tuple<bool, double, std::string>()>>();

        ASSERT_EQ(0, lua_gettop(L));

        auto results = f();
        ASSERT_TRUE(std::get<0>(results));
        ASSERT_EQ(1, std::get<1>(results));
        ASSERT_EQ("true", std::get<2>(results));
    }
    lua_close(L);
}

/*
int testScope(lua_State *L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i) {
        std::cout << i << ": " << luaL_typename(L, i) << std::endl;
    }
    lua_settop(L, 0);
    lua_pushnumber(L, 42);
    return 1;
}

TEST(StackTest, testMultipleScopes) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Stack s(L);

    lua_pushcfunction(L, testScope);
    lua_setglobal(L, "testTop");

    luaL_dostring(L, ""
                     "function scope1(n1, n2, n3)\n"
                     "  testTop(123, 23, 123)\n"
                     "  return n2\n"
                     "end\n");
    {
        lua_getglobal(L, "scope1");
        auto scope1 = s.pop<LuaFunction<int(int, int, int)>>();
        int r = scope1(1, 42, 3);
        ASSERT_EQ(42, r);
    }

    lua_close(L);
}
*/
