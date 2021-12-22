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

#ifndef TLW_METATABLE_HPP
#define TLW_METATABLE_HPP

#include <unordered_map>
#include <functional>
#include <tlw/type.hpp>
#include <tlw/stack.hpp>
#include "table.hpp"



namespace tlw {

    using lib_load_t = reference (*)(state);

    using argument_matcher = bool (*)(lua_State *);

    template<typename _user_type>
    using type_safe_method = std::tuple<argument_matcher, int (*)(stack &s, _user_type, const char *)>;

    using type_safe_function = std::tuple<argument_matcher, cfunction_t::type>;

    using type_safe_function_overloads = std::vector<type_safe_function>;

    template<typename _user_type>
    using type_safe_method_overloads = std::vector<type_safe_method<_user_type>>;

    template<typename f_type>
    using arg_counted_overloads = std::unordered_map<int, f_type>;

    using type_safe_named_function_overloads = std::unordered_map<std::string_view, arg_counted_overloads<type_safe_function_overloads>>;

    template<typename _user_type>
    using type_safe_properties = std::unordered_map<std::string_view, arg_counted_overloads<type_safe_method_overloads<_user_type>>>;

    template<class _user_type>
    struct metatable : public table {
        using user_type = typename remove_ref<_user_type>::type;
        static inline const char *name = nullptr;
        static inline arg_counted_overloads<type_safe_function_overloads> constructors{};
        static inline type_safe_properties<user_type> methods{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> setters{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> getters{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> operators{};
        static inline type_safe_function_overloads add_operator{};
        static inline type_safe_function_overloads sub_operator{};
        static inline type_safe_function_overloads mul_operator{};
        static inline type_safe_function_overloads div_operator{};
        static inline type_safe_function_overloads mod_operator{};
        static inline type_safe_function_overloads eq_operator{};
        static inline type_safe_function_overloads lt_operator{};
        static inline type_safe_function_overloads le_operator{};

        static constexpr void reset() {
            reset(false);
        }

        static constexpr void reset(bool cleanup) {
            if (cleanup && name)
                delete[] name;
            name = nullptr;
            methods.clear();
            setters.clear();
            getters.clear();
            operators.clear();
        }

        metatable(state L) : table(std::move(L)) {}

        metatable(const reference &other) : _table(other) {}

        metatable(reference &&other) : _table(other) {}

//        template<typename property_type>
//        void prop(const char * name, property_type _user_type::* property) {
//            auto indexer = get<table>("__properties");
//            tlw::table ref = indexer[name];
//            if (!ref) {
//                lua_createtable(L, 0, 2);
//                ref = tlw::table(L);
//
//            }
//
//        }

    };

    template<class _user_type>
    struct meta_table_registry {
        using type = _user_type;
        inline static const char *name = nullptr;

        static constexpr void reset() {
            name = nullptr;
            metatable<_user_type>::reset();
            metatable<_user_type *>::reset(true);
            metatable<const _user_type *>::reset(true);
        }
    };

}

#endif //TLW_METATABLE_HPP
