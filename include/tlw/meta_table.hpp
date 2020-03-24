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

namespace tlw {

    template<class _lua_user_type>
    struct meta_table_t {
        using type = _lua_user_type;
        inline static const char *name = nullptr;
        inline static cfunction_t::type ctor = nullptr;
        inline static cfunction_t::type dtor = nullptr;
        inline static cfunction_t::type index = nullptr;
        inline static cfunction_t::type new_index = nullptr;
        inline static std::unordered_map<const char *, cfunction_t::type> methods{};
        inline static std::unordered_map<const char *, cfunction_t::type> properties{};
        inline static std::unordered_map<const char *, cfunction_t::type> operators{};

        static void reset() {
            name = nullptr;
            ctor = dtor = index = new_index = nullptr;
            methods.clear();
            properties.clear();
            operators.clear();
        }
    };
}

#endif //TLW_META_TABLE_HPP
