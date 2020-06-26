//
//
#include "high_end_user_test.h"

TEST_F(high_end_user_test, test_vector_from_and_to_cpp) {

    luaL_openlibs(L);
    // Test push
    std::vector<double> values = {1.0, 2.0};
    lua["v1"] = values;

    lua.src("for i, v in ipairs(v1) do print(i,v) v1[i] = v+1 end")();

    std::vector<double> results = lua["v1"];

    ASSERT_EQ(2, results.at(0));
    ASSERT_EQ(3, results.at(1));
}

TEST_F(high_end_user_test, test_vector_from_lua_to_cpp) {

    luaL_openlibs(L);

    lua.src("v1 = { 2, 3 }")();

    std::vector<double> results = lua["v1"];

    ASSERT_EQ(2, results.at(0));
    ASSERT_EQ(3, results.at(1));
}

std::vector<double> test_f(const std::vector<double>& v) {
    printf("Is empty %d\n", v.empty());
    auto copy = v;
    for (auto& n : copy) {
        n += 1;
    }
    return copy;
}

TEST_F(high_end_user_test, test_vector_from_cpp_func) {

    luaL_openlibs(L);

    lua["f"] = test_f;

    lua.src("v1 = f{ 1, 2 }")();

    std::vector<double> results = lua["v1"];

    ASSERT_EQ(2, results.at(0));
    ASSERT_EQ(3, results.at(1));
}

TEST_F(high_end_user_test, test_unordered_map_from_and_to_cpp) {

    luaL_openlibs(L);
    // Test push
    std::unordered_map<std::string, double> values = {{"key1", 1.0}, {"key2",2.0}};
    lua["m1"] = values;

    lua.src("for k, v in pairs(m1) do print(k,v) m1[k] = v+1 end")();

    std::unordered_map<std::string, double> results = lua["m1"];

    ASSERT_EQ(2, results.at("key1"));
    ASSERT_EQ(3, results.at("key2"));
}

TEST_F(high_end_user_test, test_unordered_map_from_lua_to_cpp) {

    luaL_openlibs(L);

    lua.src("m1 = { key1 = 2, key2 = 3 }")();

    std::unordered_map<std::string, double> results = lua["m1"];

    ASSERT_EQ(2, results.at("key1"));
    ASSERT_EQ(3, results.at("key2"));
}

template<typename _key>
std::unordered_map<_key, double> test_f2(const std::unordered_map<_key, double>& v) {
    printf("Is empty %d\n", v.empty());
    auto copy = v;
    for (auto& n : copy) {
        n.second += 1;
    }
    return copy;
}

TEST_F(high_end_user_test, test_unordered_map_from_cpp_func) {

    luaL_openlibs(L);

    lua["f"] = test_f2<std::string>;

    lua.src("m1 = f{ key1 = 1, key2 = 2 }")();

    std::unordered_map<std::string, double> results = lua["m1"];

    ASSERT_EQ(2, results.at("key1"));
    ASSERT_EQ(3, results.at("key2"));
}

TEST_F(high_end_user_test, test_unordered_map_from_cpp_func_numeric) {

    luaL_openlibs(L);

    lua["f"] = test_f2<unsigned int>;

    lua.src("m1 = f{ 1, 2 }")();

    std::unordered_map<unsigned int, double> results = lua["m1"];

    ASSERT_EQ(2, results.at(1));
    ASSERT_EQ(3, results.at(2));
}

std::array<double, 2> test_f3(const std::array<double, 2>& v) {
    printf("Is empty %d\n", v.empty());
    auto copy = v;
    for (auto& n : copy) {
        n += 1;
    }
    return copy;
}

TEST_F(high_end_user_test, test_array) {

    luaL_openlibs(L);

    lua["f"] = test_f3;

    lua.src("m1 = f{ 1, 2 }")();

    std::array<double, 2> results = lua["m1"];

    ASSERT_EQ(2, results.at(0));
    ASSERT_EQ(3, results.at(1));
}
