//
// Created by enijenhuis on 30-12-2019.
//
#include "benchmark.h"
#include "classic.h"
#include "impl.h"

std::vector<Benchmark> benchmarks = {
        // TODO cfunction, lua function, pretty method, method
        {"Property Get",     "local num = example.number",      new ClassicPropertyBenchmark(),            new ImplPropertyBenchmark()},
        {"Property Set",     "example.number = 5252",           new ClassicPropertyBenchmark(),            new ImplPropertyBenchmark()},
        {"Getter",           "local num = example.number",      new ClassicGetterSetterBenchmark(),        new ImplGetterSetterBenchmark()},
        {"Setter",           "example.number = 5252",           new ClassicGetterSetterBenchmark(),        new ImplGetterSetterBenchmark()},
        {"Get method",       "local num = example:getNumber()", new ClassicGetterSetterAsTableBenchmark(), new ImplGetterSetterFastBenchmark()},
        {"Set method",       "example:setNumber(5252)",         new ClassicGetterSetterAsTableBenchmark(), new ImplGetterSetterFastBenchmark()},
        {"Fetching results", "return 1.5, true, 'string'",      new ClassicScriptResultsBenchmark(),       new ImplScriptResultsBenchmark()}
};

int main(int argc, char **argv) {
    for (auto b : benchmarks) {
        b.run();
    }
    return 0;
}
