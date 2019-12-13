//
// Created by enijenhuis on 11-12-2019.
//
#include "test.h"

Lua lua;
std::unordered_map<const std::type_info*, std::string> MetaTable::metatables = {};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    lua.file("noop", "scripts/noop.lua");
    lua.file("array", "scripts/return_array.lua");
    lua.file("table", "scripts/return_table.lua");
    lua.file("primitives", "scripts/return_primitives.lua");
    lua.file("single", "scripts/single_return.lua");

    return RUN_ALL_TESTS();
}
