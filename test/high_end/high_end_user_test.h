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

#ifndef TLW_HIGH_END_USER_TEST_H
#define TLW_HIGH_END_USER_TEST_H

#include "../base_test.h"
#include "high_end_examples.h"

class high_end_user_test : public base_test {
protected:

    tlw::lua lua = tlw::lua(tlw::state::invalid_state());

    void SetUp() override {
        base_test::SetUp();
        lua = tlw::lua(L);
        lua["vec4"] = load_vec4(L);
        lua["mat4"] = load_mat4(L);
        lua["entity"] = load_entity(L);
    }

    void TearDown() override {
        lua = tlw::lua(tlw::state::invalid_state());
        base_test::TearDown();
        tlw::meta_table_registry<tlw::example>::reset();
        tlw::meta_table_registry<tlw::entity>::reset();
        tlw::meta_table_registry<tlw::vec4>::reset();
        tlw::meta_table_registry<tlw::mat4>::reset();
    }
};

#endif //TLW_HIGH_END_USER_TEST_H
