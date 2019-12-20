//
// Created by enijenhuis on 11-12-2019.
//
#include "test.h"

Lua lua;
std::unordered_map<const std::type_info*, std::string> MetaTable::metatables = {};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
