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

#include "test.h"

TEST(ClassTest, testNormalClass) {
    ClassExample example {5.0};
    lua.setObject("example", &example)
        .src("example.number = 6.0; print(getmetatable(example))")();
    ASSERT_EQ(6.0, example.number);
}

TEST(ClassTest, testConstClass) {
    const ClassExample example {5.0};
    lua.setObject("example", &example)
    .src("result = example.number")();
    ASSERT_EQ(5.0, lua.get<double>("result"));
}

TEST(ClassTest, testConstRefClass) {
    const ClassExample example {5.0};
    lua.setObject("example", example)
            .src("result = example.number")();
    ASSERT_EQ(5.0, lua.get<double>("result"));
}

TEST(ClassTest, testRefClass) {
    ClassExample example {5.0};
    lua.setObject("example", &example)
            .src("example.number = 6.0")();
    ASSERT_EQ(6.0, example.number);
}
