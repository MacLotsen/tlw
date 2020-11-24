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

#include "high_end_user_test.h"

namespace tlw {

    class method_example {
        float number;
        const char * string;
    public:
        method_example(float n, const char *s) : number(n), string(s) {}

        void read_noop() const {
            printf("%s %f\n", string, number);
        }

        void write_noop() {
            printf("%s %f\n", string, ++number);
        }

        float write_get() {
            return number++;
        }

        const char * read_get() const {
            return string;
        }

        void write_add(float addition) {
            number += addition;
        }

        void read_add(float addition) const {
            printf("%s: %f + %f = %f\n", string, number, addition, number + addition);
        }

        float write_get_add(float addition) {
            write_add(addition);
            return number;
        }

        float read_get_add(float addition) const {
            return number + addition;
        }
    };

}

TEST_F(high_end_user_test, test_method) {
    luaL_openlibs(L);
    lua_settop(L, 0);
    tlw::define<tlw::method_example>("method_example")
            .method("read_noop", &tlw::method_example::read_noop)
            .method("write_noop", &tlw::method_example::write_noop)
            .method("read_get", &tlw::method_example::read_get)
            .method("write_get", &tlw::method_example::write_get)
            .method("write_add", &tlw::method_example::write_add)
            .method("read_add", &tlw::method_example::read_add)
            .method("write_get_add", &tlw::method_example::write_get_add)
            .method("read_get_add", &tlw::method_example::read_get_add)
            .build()(L);

    tlw::method_example e1(5.5, "writable");
    const tlw::method_example e2(5.5, "readonly");

    auto test_rn = lua.src("e.read_noop()");
    auto test_wn = lua.src("e.write_noop()");
    auto test_rg = lua.src("print(e.read_get())");
    auto test_wg = lua.src("print(e.write_get())");
    auto test_wa = lua.src("e.write_add(5)");
    auto test_ra = lua.src("e.read_add(5)");
    auto test_wga = lua.src("print(e.write_get_add(5))");
    auto test_rga = lua.src("print(e.read_get_add(5))");

    // Test non-const as value
    // Values are copied
    lua.set("e", e1);
    ASSERT_NO_THROW(test_rn());
    ASSERT_NO_THROW(test_wn());
    ASSERT_NO_THROW(test_rg());
    ASSERT_NO_THROW(test_wg());
    ASSERT_NO_THROW(test_wa());
    ASSERT_NO_THROW(test_ra());
    ASSERT_NO_THROW(test_wga());
    ASSERT_NO_THROW(test_rga());

    // Test non-const as pointer
    lua.set("e", &e1);
    ASSERT_NO_THROW(test_rn());
    ASSERT_NO_THROW(test_wn());
    ASSERT_NO_THROW(test_rg());
    ASSERT_NO_THROW(test_wg());
    ASSERT_NO_THROW(test_wa());
    ASSERT_NO_THROW(test_ra());
    ASSERT_NO_THROW(test_wga());
    ASSERT_NO_THROW(test_rga());

    // Test const as value
    // Values are copied
    lua.set("e", e2);
    ASSERT_NO_THROW(test_rn());
    ASSERT_NO_THROW(test_wn());
    ASSERT_NO_THROW(test_rg());
    ASSERT_NO_THROW(test_wg());
    ASSERT_NO_THROW(test_wa());
    ASSERT_NO_THROW(test_ra());
    ASSERT_NO_THROW(test_wga());
    ASSERT_NO_THROW(test_rga());

    // Test non-const as pointer
    lua.set("e", &e2);
    ASSERT_NO_THROW(test_rn());
    ASSERT_ANY_THROW(test_wn());
    ASSERT_NO_THROW(test_rg());
    ASSERT_ANY_THROW(test_wg());
    ASSERT_ANY_THROW(test_wa());
    ASSERT_NO_THROW(test_ra());
    ASSERT_ANY_THROW(test_wga());
    ASSERT_NO_THROW(test_rga());

    tlw::meta_table_registry<tlw::method_example>::reset();
}
