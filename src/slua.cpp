//
// Created by enijenhuis on 24-12-2019.
//
#include <slua/metatable.h>

std::unordered_map<const std::type_info*, const ClassPrototype*> MetaTable::metaTables = {};
std::unordered_map<const std::type_info*, const PrettyClassPrototype*> MetaTable::prettyTables = {};
