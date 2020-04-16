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

#ifndef TLW__TABLE_HPP
#define TLW__TABLE_HPP

namespace tlw {
    template<bool is_global>
    struct _table {
    };

    using table = _table<false>;
    using global_table = _table<true>;

    template<bool is_global>
    struct stack_traits<_table<is_global>> : public reference_stack_traits<table_t> {

    };
}

#endif //TLW__TABLE_HPP
