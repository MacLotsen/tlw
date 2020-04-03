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

#ifndef TLW_USER_TEST_H
#define TLW_USER_TEST_H

#include "base_test.h"

class user_test : public base_test {
    using example_meta = tlw::meta_table_registry<tlw::lua_example_t::type>;
protected:
    tlw::stack s;
    void SetUp() override {
        tlw::meta_table_registry<tlw::example>::reset();
        base_test::SetUp();
        s = tlw::stack(L);
    }

    void TearDown() override {
        base_test::TearDown();
        example_meta::reset();
        s = tlw::stack();
    }
};

#endif //TLW_USER_TEST_H
