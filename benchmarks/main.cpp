//
// Created by enijenhuis on 30-12-2019.
//
#include "benchmark.h"
#include "classic.h"
#include "impl.h"

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
    for (auto b : benchmarks) {
        b.run();
    }
    return 0;
}
