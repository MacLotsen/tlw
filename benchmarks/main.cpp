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

#include "benchmark.h"
#include "classic.h"
#include "impl.h"

#define VERSION_TO_STRING(x) #x
#define VERSION_STR(x) VERSION_TO_STRING(x)

static const char *disclaimer = "TLW version " VERSION_STR(TLW_VERSION) ", Copyright (C) 2019 Erik Nijenhuis\n"
                                "TLW comes with ABSOLUTELY NO WARRANTY. This is free\n"
                                "software, and you are welcome to redistribute it un-\n"
                                "der certain conditions.\n";


std::vector<Benchmark> benchmarks = {
        {"Set global bool",      "",                                new ClassicGlobalBooleanSetBenchmark,    new ImplGlobalBooleanSetBenchmark,  52000000},
        {"Get global bool",      "b = true",                        new ClassicGlobalBooleanGetBenchmark,    new ImplGlobalBooleanGetBenchmark,  45000000},
        {"Set global number",    "",                                new ClassicGlobalNumberSetBenchmark,     new ImplGlobalNumberSetBenchmark,   55000000},
        {"Get global number",    "n = 2.5",                         new ClassicGlobalNumberGetBenchmark,     new ImplGlobalNumberGetBenchmark,   46000000},
        {"Set global string",    "",                                new ClassicGlobalStringSetBenchmark,     new ImplGlobalStringSetBenchmark,   36000000},
        {"Get global string",    "s = 'str'",                       new ClassicGlobalStringGetBenchmark,     new ImplGlobalStringGetBenchmark,   43000000},
        {"Set global function",  "",                                new ClassicGlobalFunctionSetBenchmark,   new ImplGlobalFunctionSetBenchmark, 22000000},
        {"Get global function",  "function f() return true end",    new ClassicGlobalFunctionGetBenchmark,   new ImplGlobalFunctionGetBenchmark, 19000000},
        {"Property Get",         "local num = example.number",      new ClassicPropertyBenchmark,            new ImplPropertyBenchmark,          11500000},
        {"Property Set",         "example.number = 5252",           new ClassicPropertyBenchmark,            new ImplPropertyBenchmark,          11000000},
        {"Getter",               "local num = example.number",      new ClassicGetterSetterBenchmark,        new ImplGetterSetterBenchmark,      11500000},
        {"Setter",               "example.number = 5252",           new ClassicGetterSetterBenchmark,        new ImplGetterSetterBenchmark,      11000000},
        {"Get method",           "local num = example:getNumber()", new ClassicGetterSetterAsTableBenchmark, new ImplGetterSetterFastBenchmark,  19000000},
        {"Set method",           "example:setNumber(5252)",         new ClassicGetterSetterAsTableBenchmark, new ImplGetterSetterFastBenchmark,  17000000},
        {"Fetching results",     "return 1.5, true, 'string'",      new ClassicScriptResultsBenchmark,       new ImplScriptResultsBenchmark,     17000000},
        {"Fetching table key",   "t = {r = 5, g = 3, b = 1}",       new ClassicTableFetchBenchmark,          new ImplTableFetchBenchmark,        48000000},
        {"Fetching table keys",  "t = {r = 5, g = 3, b = 1}",       new ClassicTableFetchesBenchmark,        new ImplTableFetchesBenchmark,      18000000},
        {"Fetching list key",    "t = { 5, 3, 1 }",                 new ClassicListFetchBenchmark,           new ImplListFetchBenchmark,         62000000},
        {"Fetching list keys",   "t = { 5, 3, 1 }",                 new ClassicListFetchesBenchmark,         new ImplListFetchesBenchmark,       27000000},
        {"Setting table value",  "t = {}",                          new ClassicTableSetBenchmark,            new ImplTableSetBenchmark,          50000000},
        {"Setting table values", "t = {}",                          new ClassicTableSetsBenchmark,           new ImplTableSetsBenchmark,         20000000},
};

int main(int argc, char **argv) {
    std::cout << disclaimer << std::endl;
    for (auto b : benchmarks) {
        b.run();
    }
    return 0;
}
