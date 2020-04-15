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

#ifndef TLW_UTIL_H
#define TLW_UTIL_H

namespace tlw {

    template<typename _type, typename ...>
    struct pointer_type {
        static constexpr bool valid = false;
        using value_type = _type;
    };

    template<typename _type>
    struct pointer_type<_type*> {
        static constexpr bool valid = true;
        using value_type = _type;
    };

    template<typename _type, typename ...>
    struct const_type {
        static constexpr bool valid = false;
        using value_type = _type;

    };

    template<typename _type>
    struct const_type<const _type> {
        static constexpr bool valid = true;
        using value_type = _type;
    };

    template<typename ...>
    struct method_type {

    };

    template<typename _class, typename _r, typename ..._args>
    struct method_type<_r (_class::*)(_args...)> {
        constexpr static inline bool read_only = false;
        constexpr static inline int arg_count = sizeof...(_args);
    };

    template<typename _class, typename ..._args>
    struct method_type<void (_class::*)(_args...)> {
        constexpr static inline bool read_only = false;
        constexpr static inline int arg_count = sizeof...(_args);
    };

    template<typename _class, typename _r>
    struct method_type<_r (_class::*)()> {
        constexpr static inline bool read_only = false;
        constexpr static inline int arg_count = 0;
    };

    template<typename _class>
    struct method_type<void (_class::*)()> {
        constexpr static inline bool read_only = false;
        constexpr static inline int arg_count = 0;
    };

    template<typename _class, typename _r, typename ..._args>
    struct method_type<_r (_class::*)(_args...) const> {
        constexpr static inline bool read_only = true;
        constexpr static inline int arg_count = sizeof...(_args);
    };

    template<typename _class, typename ..._args>
    struct method_type<void (_class::*)(_args...) const> {
        constexpr static inline bool read_only = true;
        constexpr static inline int arg_count = sizeof...(_args);
    };

    template<typename _class, typename _r>
    struct method_type<_r (_class::*)() const> {
        constexpr static inline bool read_only = true;
        constexpr static inline int arg_count = 0;
    };

    template<typename _class>
    struct method_type<void (_class::*)() const> {
        constexpr static inline bool read_only = true;
        constexpr static inline int arg_count = 0;
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

}

#endif //TLW_UTIL_H
