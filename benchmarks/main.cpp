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

static const char * disclaimer = "TLW version " VERSION_STR(TLW_VERSION) ", Copyright (C) 2019 Erik Nijenhuis\n"
                                 "TLW comes with ABSOLUTELY NO WARRANTY. This is free\n"
                                 "software, and you are welcome to redistribute it un-\n"
                                 "der certain conditions.\n";


std::vector<Benchmark> benchmarks = {
        {"Property Get",         "local num = example.number",      new ClassicPropertyBenchmark,            new ImplPropertyBenchmark},
        {"Property Set",         "example.number = 5252",           new ClassicPropertyBenchmark,            new ImplPropertyBenchmark},
        {"Getter",               "local num = example.number",      new ClassicGetterSetterBenchmark,        new ImplGetterSetterBenchmark},
        {"Setter",               "example.number = 5252",           new ClassicGetterSetterBenchmark,        new ImplGetterSetterBenchmark},
        {"Get method",           "local num = example:getNumber()", new ClassicGetterSetterAsTableBenchmark, new ImplGetterSetterFastBenchmark},
        {"Set method",           "example:setNumber(5252)",         new ClassicGetterSetterAsTableBenchmark, new ImplGetterSetterFastBenchmark},
        {"Fetching results",     "return 1.5, true, 'string'",      new ClassicScriptResultsBenchmark,       new ImplScriptResultsBenchmark},
        {"Fetching table key",   "t = {r = 5, g = 3, b = 1}",       new ClassicTableFetchBenchmark,          new ImplTableFetchBenchmark},
        {"Fetching table keys",  "t = {r = 5, g = 3, b = 1}",       new ClassicTableFetchesBenchmark,        new ImplTableFetchesBenchmark},
        {"Fetching list key",    "t = { 5, 3, 1 }",                 new ClassicListFetchBenchmark,           new ImplListFetchBenchmark},
        {"Fetching list keys",   "t = { 5, 3, 1 }",                 new ClassicListFetchesBenchmark,         new ImplListFetchesBenchmark},
        {"Setting table value",  "t = {}",                          new ClassicTableSetBenchmark,            new ImplTableSetBenchmark},
        {"Setting table values", "t = {}",                          new ClassicTableSetsBenchmark,           new ImplTableSetsBenchmark},
};

int main(int argc, char **argv) {
    std::cout << disclaimer << std::endl;
    for (auto b : benchmarks) {
        b.run();
    }
    return 0;
}
