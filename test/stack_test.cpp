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

#include "detail_test.h"

TEST_F(detail_test, test_stack) {
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
