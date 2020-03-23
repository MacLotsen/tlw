/*
 * Typed Lua Wrapping
 * Copyright (C) 2019  Erik Nijenhuis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <gtest/gtest.h>
#include <tlw/types/type_traits.h>
#include <tlw/util.hpp>
#include <unordered_map>

namespace tlw {

    template<class _lua_user_type>
    struct lua_metatable_t {
        using type = _lua_user_type;
        inline static const char *name = nullptr;
        inline static lua_cfunction_t::type ctor = nullptr;
        inline static lua_cfunction_t::type dtor = nullptr;
        inline static lua_cfunction_t::type index = nullptr;
        inline static lua_cfunction_t::type new_index = nullptr;
        inline static std::unordered_map<const char *, lua_cfunction_t::type> methods{};
        inline static std::unordered_map<const char *, lua_cfunction_t::type> properties{};
        inline static std::unordered_map<const char *, lua_cfunction_t::type> operators{};
    };

    template<typename ...>
    struct stack_traits {

    };

    template<typename _lua_type>
    struct primitive_stack_traits {
        static constexpr void push(lua_State *L, typename _lua_type::type value) {
            lua_type_traits<_lua_type>::push(L, value);
        }

        static constexpr typename _lua_type::type peek(lua_State *L, int idx) {
            return lua_type_traits<_lua_type>::peek(L, idx);
        }
    };

    template<>
    struct stack_traits<lua_none_t::type> {
        static constexpr void push(lua_State *L) {
            lua_type_traits<lua_none_t>::push(L);
        }

        static constexpr lua_none_t::type peek(lua_State *L, int idx) {
            return lua_type_traits<lua_none_t>::peek(L, idx);
        }
    };

    template<>
    struct stack_traits<lua_nil_t::type> : public primitive_stack_traits<lua_nil_t> {
    };

    template<>
    struct stack_traits<lua_bool_t::type> : public primitive_stack_traits<lua_bool_t> {
    };

    template<>
    struct stack_traits<lua_number_t::type> : public primitive_stack_traits<lua_number_t> {
    };

    template<>
    struct stack_traits<float> : public primitive_stack_traits<lua_number_t> {
    };

    template<>
    struct stack_traits<int> : public primitive_stack_traits<lua_number_t> {
    };

    template<>
    struct stack_traits<long> : public primitive_stack_traits<lua_number_t> {
    };

    template<>
    struct stack_traits<lua_string_t::type> : public primitive_stack_traits<lua_string_t> {
    };

    template<class T>
    struct stack_traits<T> {
        using _user_data_t = lua_user_data_t<T>;
        using _light_user_data_t = lua_light_user_data_t<T>;

        static void push(lua_State *L, T value) {
            if (lua_metatable_t<typename _user_data_t::type>::name) {
                if constexpr (pointer_type<T>::valid) {
                    lua_type_traits<_user_data_t>::push(L, value);
                } else {
                    lua_type_traits<_user_data_t>::push(L, std::move(value));
                }
                if (luaL_newmetatable(L, lua_metatable_t<typename _user_data_t::type>::name)) {
                    // TODO create meta table
                }
                lua_setmetatable(L, -2);
            } else {
//                lua_type_traits<_light_user_data_t>()(L, value);
            }
        }

        static T peek(lua_State *L, int idx) {
            if (lua_metatable_t<T>::name) {
                return lua_type_traits<_user_data_t>::peek(L, idx);
            } else {
//                return lua_type_traits<_light_user_data_t>()(L, idx);
            }
            return nullptr;
        }
    };


    template<typename ..._args>
    struct lua_function<void(_args...)> : public lua_struct_ref {
        void operator()(_args ...args) {
            stack_traits<lua_struct_ref *>::push(L, this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 0, 0)) {
                throw std::runtime_error("Failed to call lua function.");
            }
            lua_settop(L, 0);
        }
    };

    template<typename _r, typename ..._args>
    struct lua_function<_r(_args...)> : public lua_struct_ref {
        _r operator()(_args ...args) {
            stack_traits<lua_struct_ref *>::push(L, this);
            (..., stack_traits<_args>::push(L, args));
            if (lua_pcall(L, sizeof...(_args), 1, 0)) {
                throw std::runtime_error("Failed to call lua function.");
            }
            _r r = stack_traits<_r>::peek(L, -1);
            lua_pop(L, 1);
            return r;
        }
    };

    class stack {
        lua_State *L;
    public:
        explicit stack(lua_State *L) : L(L) {}

        template<typename ...Ts>
        void place(Ts ...values) {
            (..., push<Ts>(values));
        }

        template<class T>
        void push(T value) {
            if constexpr(pointer_type<T>::valid) {
                stack_traits<T>::push(L, value);
            } else {
                stack_traits<T>::push(L, std::move(value));
            }
        }

        template<class T>
        T peek(int idx) {
            return stack_traits<T>::peek(L, idx);
        }

        template<class T>
        T pop() {
            T val = peek<T>(-1);
            lua_pop(L, 1);
            return val;
        }

        template<class T>
        T pick(int idx) {
            T val = peek<T>(idx);
            lua_remove(L, idx);
            return val;
        }

        template<class ...Ts>
        std::tuple<Ts...> grab() {
            int begin = lua_gettop(L) + 1 - sizeof...(Ts);
            int idx = begin;
            auto values = std::tuple{ordered_peek<Ts>(idx)...};
            lua_settop(L, begin - 1);
            return values;
        }

    private:
        template<class T>
        T ordered_peek(int &idx) {
            return peek<T>(idx++);
        }

    };

    template<typename ...>
    struct lua_binding {
        virtual lua_cfunction_t::type operator()() {
            return nullptr;
        };
    };

    template<class _user_type, typename ..._args>
    struct lua_ctor_binder_t : public lua_binding<_user_type, _args...> {
        lua_cfunction_t::type operator()() override {
            return [](lua_State *L) -> int {
                if (sizeof...(_args)) {
                    auto params = stack(L).grab<_args...>();
                    auto seq = gen_seq<sizeof...(_args)>();
                    if constexpr (pointer_type<_user_type>::valid) {
                        lua_type_traits<lua_user_data_t<_user_type>>::push(L, create_pointer(params, seq));
                    } else {
                        lua_type_traits<lua_user_data_t<_user_type>>::push(L, create_value(params, seq));
                    }
                } else {
                    if constexpr (pointer_type<_user_type>::valid) {
                        lua_type_traits<lua_user_data_t<_user_type>>::push(L,
                                                                           new typename pointer_type<_user_type>::value_type());
                    } else {
                        lua_type_traits<lua_user_data_t<_user_type>>::push(L, _user_type());
                    }
                }
                return 1;
            };
        }

        template<int ...Ns>
        constexpr static _user_type create_pointer(std::tuple<_args...> t, seq<Ns...>) {
            return new typename pointer_type<_user_type>::value_type(std::get<Ns>(t)...);
        }

        template<int ...Ns>
        constexpr static _user_type create_value(std::tuple<_args...> t, seq<Ns...>) {
            return _user_type(std::get<Ns>(t)...);
        }
    };

    // TODO mk_class(name, bool traditional)
    // TODO mk_struct (different ctor)
    // TODO mk_function
    // TODO mk_method
    // TODO mk_property

    template<class T>
    constexpr void define(const char *name) {
        if (lua_metatable_t<T>::name) {
            printf("Warning: meta table %s already defined\n", lua_metatable_t<T>::name);
            if (strcmp(lua_metatable_t<T>::name, name) != 0) {
                printf("Warning: changing meta table name from %s to %s\n", lua_metatable_t<T>::name, name);
            }
        }
        lua_metatable_t<T>::name = name;
    }

    template<class _user_type, typename ..._args>
    constexpr void define_ctor() {
        if (!lua_metatable_t<_user_type>::name) {
            printf("Warning: constructor for %s already defined", lua_metatable_t<_user_type>::name);
        } else if (lua_metatable_t<_user_type>::ctor) {
            printf("Warning: redefining constructor for %s", lua_metatable_t<_user_type>::name);
        }
        lua_metatable_t<_user_type>::ctor = lua_ctor_binder_t<_user_type, _args...>()();
    }

    struct example {
        float val;

        explicit example() : val(5.5) {
            printf("example created\n");
        }

        example(const example &e) : val(e.val) {
            printf("example copied\n");
        }

        example(example &&e) noexcept : val(std::exchange(e.val, 0)) {
            printf("example moved\n");
        }

        explicit example(float val) : val(val) {
            printf("example created with %g\n", val);
        }

        example operator=(example &&e) noexcept {
            return example(std::move(e));
        }

        example operator=(const example &e) {
            return example(e);
        }

        virtual ~example() {
            printf("example destroyed\n");
        }
    };

    struct example2 {
    };

    using lua_example_t = lua_user_data_t<const example *>;
    using lua_invalid_example_t = lua_user_data_t<const example2 *>;
}

class TypeTest : public ::testing::Test {
protected:
    static inline const bool magic_bool = true;
    static inline const double magic_number = 5.5;
    static inline const char *magic_string = "TLW";
    static inline const int magic_table = 1;
    static inline const int magic_function = 2;
    static inline const auto magic_user_datum = tlw::example();

    lua_State *L = nullptr;

    void SetUp() override {
        L = luaL_newstate();
    }

    void TearDown() override {
        lua_close(L);
    }

    template<typename ...Ts>
    void inspect(bool expected) {
        (..., assert_inspection<Ts>(expected));
    }

    template<typename T>
    void assert_inspection(bool expected) {
        ASSERT_EQ(expected, tlw::lua_type_inspector<T>::inspect(L));
    }

    static int noop(lua_State *) { return 0; }
};

TEST_F(TypeTest, testMoveValue) {
    tlw::stack s(L);
    tlw::define<tlw::example>("example_value");
    tlw::example value;
    s.push(std::move(value));
    ASSERT_FALSE(tlw::lua_type_inspector<tlw::lua_nil_t>::inspect(L));
    ASSERT_TRUE(tlw::lua_type_inspector<tlw::lua_user_data_t<const tlw::example *>>::inspect(L));
}

TEST_F(TypeTest, testPointerOwnership) {
    tlw::stack s(L);
    tlw::define<const tlw::example *>("example");
    tlw::define_ctor<const tlw::example *>();
    lua_pushcfunction(L, tlw::lua_metatable_t<const tlw::example *>::ctor);
    if (lua_pcall(L, 0, 1, 0)) {
        FAIL() << "Failed to call constructor";
    }
    ASSERT_TRUE(tlw::lua_type_inspector<tlw::lua_example_t>::inspect(L));
    auto created_example = tlw::lua_type_traits<tlw::lua_example_t>::peek(L, -1);
    delete created_example;

    tlw::define_ctor<const tlw::example *, float>();
    lua_pushcfunction(L, tlw::lua_metatable_t<const tlw::example *>::ctor);
    s.push(5.5);
    if (lua_pcall(L, 1, 1, 0)) {
        FAIL() << "Failed to call constructor with argument";
    }
    ASSERT_TRUE(tlw::lua_type_inspector<tlw::lua_example_t>::inspect(L));
    created_example = tlw::lua_type_traits<tlw::lua_example_t>::peek(L, -1);
    delete created_example;
}

TEST_F(TypeTest, testStack) {
    tlw::stack s(L);
    tlw::define<const tlw::example *>("example");

    s.push(magic_bool);
    ASSERT_EQ(magic_bool, s.peek<bool>(-1));
    s.push(magic_number);
    ASSERT_EQ(magic_number, s.peek<double>(-1));
    s.push(magic_string);
    ASSERT_STREQ(magic_string, s.peek<const char *>(-1));
    s.push(&magic_user_datum);
    ASSERT_EQ(&magic_user_datum, s.peek<decltype(&magic_user_datum)>(-1));

    auto stack_values = s.grab<const char *, decltype(&magic_user_datum)>();
    ASSERT_EQ(magic_bool, s.peek<bool>(1));
    ASSERT_EQ(magic_number, s.peek<double>(2));
    ASSERT_STREQ(magic_string, std::get<0>(stack_values));
    ASSERT_EQ(&magic_user_datum, std::get<1>(stack_values));
}

TEST_F(TypeTest, testTypeInspector) {
    lua_pushboolean(L, magic_bool);
    inspect<tlw::lua_bool_t>(true);
    inspect<tlw::lua_number_t, tlw::lua_string_t, tlw::lua_table_t, tlw::lua_cfunction_t, tlw::lua_example_t>(false);

    lua_pushnumber(L, magic_number);
    inspect<tlw::lua_number_t>(true);
    inspect<tlw::lua_bool_t, tlw::lua_string_t, tlw::lua_table_t, tlw::lua_cfunction_t, tlw::lua_example_t>(false);

    lua_pushstring(L, magic_string);
    inspect<tlw::lua_string_t>(true);
    inspect<tlw::lua_bool_t, tlw::lua_number_t, tlw::lua_table_t, tlw::lua_cfunction_t, tlw::lua_example_t>(false);

    lua_createtable(L, 0, 0);
    inspect<tlw::lua_table_t>(true);
    inspect<tlw::lua_bool_t, tlw::lua_number_t, tlw::lua_string_t, tlw::lua_cfunction_t, tlw::lua_example_t>(false);

    lua_pushcfunction(L, &noop);
    inspect<tlw::lua_cfunction_t>(true);
    inspect<tlw::lua_bool_t, tlw::lua_number_t, tlw::lua_string_t, tlw::lua_table_t, tlw::lua_example_t>(false);

    lua_newuserdata(L, sizeof(const tlw::example **));
    inspect<tlw::lua_example_t>(true);
    inspect<tlw::lua_bool_t, tlw::lua_number_t, tlw::lua_string_t, tlw::lua_table_t, tlw::lua_cfunction_t>(false);
}

TEST_F(TypeTest, testPeek) {
    lua_pushboolean(L, magic_bool);
    ASSERT_EQ(magic_bool, tlw::lua_ordered_type_traits<tlw::lua_bool_t>::peek(L));

    lua_pushnumber(L, magic_number);
    ASSERT_EQ(magic_number, tlw::lua_ordered_type_traits<tlw::lua_number_t>::peek(L));

    lua_pushstring(L, magic_string);
    ASSERT_STREQ(magic_string, tlw::lua_ordered_type_traits<tlw::lua_string_t>::peek(L));

    lua_createtable(L, 0, 0);
    auto t_ref = tlw::lua_ordered_type_traits<tlw::lua_table_t>::peek(L);
    ASSERT_TRUE(static_cast<bool>(t_ref));

    lua_pushcfunction(L, &noop);
    auto fn_ref = tlw::lua_ordered_type_traits<tlw::lua_function_t>::peek(L);
    ASSERT_TRUE(static_cast<bool>(fn_ref));

    auto user_data = (const tlw::example **) lua_newuserdata(L, sizeof(const tlw::example **));
    *user_data = &magic_user_datum;
    ASSERT_EQ(&magic_user_datum, tlw::lua_ordered_type_traits<tlw::lua_example_t>::peek(L));
}

TEST_F(TypeTest, testPush) {
    tlw::lua_type_traits<tlw::lua_bool_t>::push(L, magic_bool);
    ASSERT_EQ(magic_bool, lua_toboolean(L, -1));

    tlw::lua_type_traits<tlw::lua_number_t>::push(L, magic_number);
    ASSERT_EQ(magic_number, lua_tonumber(L, -1));

    tlw::lua_type_traits<tlw::lua_string_t>::push(L, magic_string);
    ASSERT_STREQ(magic_string, lua_tostring(L, -1));

    lua_createtable(L, 0, 0);
    auto table_ref = tlw::lua_struct_ref(L);
    tlw::lua_type_traits<tlw::lua_table_t>::push(L, std::move(table_ref));
    ASSERT_EQ(tlw::lua_table_t::value, lua_type(L, -1));

    lua_pushcfunction(L, &noop);
    auto function_ref = tlw::lua_struct_ref(L);
    tlw::lua_type_traits<tlw::lua_function_t>::push(L, std::move(function_ref));
    ASSERT_EQ(tlw::lua_function_t::value, lua_type(L, -1));

    tlw::lua_type_traits<tlw::lua_example_t>::push(L, &magic_user_datum);
    ASSERT_EQ(&magic_user_datum, *(tlw::example **) lua_touserdata(L, -1));
}
