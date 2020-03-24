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

#ifndef TLW_BASE_TEST_H
#define TLW_BASE_TEST_H

#include <gtest/gtest.h>
#include <tlw/tlw.hpp>

#include "examples.h"


class base_test : public ::testing::Test {
protected:
    lua_State *L = nullptr;

    virtual void SetUp() override {
        L = luaL_newstate();
    }

    virtual void TearDown() override {
        lua_close(L);
    }

    template<typename ...Ts>
    void inspect(bool expected) {
        (..., assert_inspection<Ts>(expected));
    }

    template<typename T>
    void assert_inspection(bool expected) {
        ASSERT_EQ(expected, tlw::type_inspector<T>::inspect(L));
    }
};

#endif //TLW_BASE_TEST_H
