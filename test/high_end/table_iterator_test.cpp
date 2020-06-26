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


TEST_F(high_end_user_test, test_iterator_indexed) {
    tlw::lua lua = tlw::lua();
    auto f = lua.src<tlw::table()>("return {'val1', 'val2', 'val3'}");
    // TODO: implement tlw::list
//    tlw::table t = f();
//    std::vector<std::string> result {};
//
//    for (auto kv : t) {
//        std::string val = kv.second;
//        result.push_back(val);
//    }
//
//    ASSERT_EQ("val1", result.at(0));
//    ASSERT_EQ("val2", result.at(1));
//    ASSERT_EQ("val3", result.at(2));
}

TEST_F(high_end_user_test, test_iterator_named) {
    tlw::lua lua = tlw::lua();
    auto f = lua.src<tlw::table()>("return {key1='val1', key2='val2', key3='val3'}");
    tlw::table t = f();
    std::vector<std::string> result {};

    for (auto kv : t) {
        std::string val = kv.second;
        result.push_back(val);
    }

    for (const auto& s : result) {
        if (s != "val1" && s != "val2" && s != "val3") {
            FAIL() << "Missing value in table '" << s << "'.";
        }
    }
}

TEST_F(high_end_user_test, test_multi_iterator_named) {
    tlw::lua lua = tlw::lua();
    auto t = lua.src<tlw::table()>("return {"
                                   "key1 = { values = { v1 = 'hi', v2 = 'hoi' }, values2 = {w1 = 'bye', w2 = 'doei'}},"
                                   "key2 = { values = { v1 = 'hi', v2 = 'hoi' }, values2 = {w1 = 'bye', w2 = 'doei'}},"
                                   "key3 = { values = { v1 = 'hi', v2 = 'hoi' }, values2 = {w1 = 'bye', w2 = 'doei'}}"
                                   "}")();
    for (auto kv : t) {
        tlw::table values = kv.second;
        printf("%d global key %s\n", lua_gettop(lua.L), kv.first);
        for (auto kv2 : values) {
            tlw::table values2 = kv2.second;
            printf("\tTop: %d, Table %s\n", lua_gettop(lua.L), kv2.first);
            for (auto kv3 : values2) {
                printf("\t\tKey %s, Value %s\n", kv3.first, (const char *) kv3.second);
            }
        }
        fflush(stdout);
    }
}
