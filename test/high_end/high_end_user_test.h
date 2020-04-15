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
        load_vec4(L);
        load_mat4(L);
        load_entity(L);
    }

    void TearDown() override {
        lua = tlw::lua(tlw::state::invalid_state());
        base_test::TearDown();
        tlw::meta_table_registry<tlw::example>::reset();
    }

    static void load_vec4(const tlw::state &L) {
        tlw::define<tlw::vec4>("vec4")
                .ctor<>()
                .ctor<float>()
                .ctor<float, float, float>()
                .ctor<float, float, float, float>()
                .prop("x", &tlw::vec4::x)
                .prop("y", &tlw::vec4::y)
                .prop("z", &tlw::vec4::z)
                .prop("w", &tlw::vec4::w)
                .finish();
        tlw::meta_table_registry<tlw::vec4>::expose(L);
    }

    static void load_mat4(const tlw::state &L) {
        tlw::define<tlw::mat4>("mat4")
                .ctor<>()
                .ctor<float>()
                .ctor<float, float, float, float,
                        float, float, float, float,
                        float, float, float, float,
                        float, float, float, float>()
                .finish();
        tlw::meta_table_registry<tlw::mat4>::expose(L);
    }

    static void load_entity(const tlw::state &L) {
        tlw::define<tlw::entity>("entity")
                .ctor<tlw::vec4>()
                .ctor<tlw::mat4>()
                .finish();
        tlw::meta_table_registry<tlw::entity>::expose(L);
    }
};

#endif //TLW_HIGH_END_USER_TEST_H
