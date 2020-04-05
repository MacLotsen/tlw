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

#ifndef TLW_META_TABLE_HPP
#define TLW_META_TABLE_HPP

#include <unordered_map>
#include <tlw/type.hpp>
#include <functional>

namespace tlw {

    using type_safe_ctor = std::tuple<bool (*)(lua_State *), cfunction_t::type>;
    using ctor_set = std::vector<type_safe_ctor>;
    using ctor_map = std::unordered_map<int, ctor_set>;

    template<class _user_type>
    struct meta_table {
        using user_type = _user_type;
        static inline const char *name = nullptr;
        static inline ctor_map constructors{};
        static inline cfunction_t::type ctor = nullptr;
        static inline cfunction_t::type dtor = nullptr;
        static inline std::unordered_map<std::string_view, cfunction_t::type> methods{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> setters{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> getters{};
        static inline std::unordered_map<std::string_view, cfunction_t::type> operators{};

        static void reset() {
            reset(false);
        }

        static void reset(bool cleanup) {
            if (cleanup && name)
                delete[] name;
            name = nullptr;
            ctor = dtor = nullptr;
            methods.clear();
            setters.clear();
            getters.clear();
            operators.clear();
        }
    };

    template<class _user_type>
    struct meta_table_registry {
        using type = _user_type;
        inline static const char *name = nullptr;

        inline static void (*expose)(state L);

        static void reset() {
            name = nullptr;
            expose = nullptr;
            meta_table<_user_type>::reset();
            meta_table<const _user_type>::reset(true);
        }
    };

}

#endif //TLW_META_TABLE_HPP
