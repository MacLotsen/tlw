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

#ifndef TLW_BENCHMARK_H
#define TLW_BENCHMARK_H

#include <chrono>
#include <iostream>
#include <iomanip>
#include <utility>
#include <tlw/old/_tlw.hpp>

class BenchmarkRunner {
public:
    virtual void prepare(const char *script) = 0;

    virtual void run() = 0;
};

class Benchmark {
    using clock = std::chrono::high_resolution_clock;
    using time = std::chrono::nanoseconds;
private:
    inline static const int columnWidth = 22;
    std::string name;
    const char *script;
    long long iterations;
    BenchmarkRunner *classicRunner, *implementationRunner;
public:
    Benchmark(std::string name, const char *script, BenchmarkRunner *c, BenchmarkRunner *i, long long iterations) :
            name(std::move(name)), script(script), classicRunner(c), implementationRunner(i), iterations(iterations) {}

    Benchmark(std::string name, const char *script, BenchmarkRunner *c, BenchmarkRunner *i) :
            Benchmark(std::move(name), script, c, i, 10000000) {}

    void run() {
        classicRunner->prepare(script);
        implementationRunner->prepare(script);

        auto classicStart = clock::now();
        for (long long i = 0; i < iterations; ++i)
            classicRunner->run();
        auto classicEnd = clock::now();

        auto implementationStart = clock::now();
        for (long long i = 0; i < iterations; ++i)
            implementationRunner->run();
        auto implementationEnd = clock::now();

        auto classicDiff = std::chrono::duration_cast<time>(classicEnd - classicStart);
        auto implementationDiff = std::chrono::duration_cast<time>(implementationEnd - implementationStart);

        double factor = classicDiff > implementationDiff
                        ? (double(implementationDiff.count()) / double(classicDiff.count()))
                        : (double(classicDiff.count()) / double(implementationDiff.count()));

        std::stringstream ss;
        ss << "Implementation is "
           << std::fixed << std::setprecision(2) << ((1 - factor) * 100)
           << "% " << (classicDiff < implementationDiff ? "slower" : "faster") << ".";

        std::cout << std::setfill('_') << std::setw(80) << "" << std::endl << std::setfill(' ')
                  << std::left << "| " << std::setw(37) << name << "|"
                  << std::setw(19) << " Lua" << "|"
                  << std::setw(19) << " TLW" << "|" << std::endl
                  << "| " << std::setw(37) << ss.str() << "|"
                  << std::fixed << std::right
                  << std::setw(5) << classicDiff.count()/1000000 << "ms |"
                  << std::setw(4) << classicDiff.count() / iterations  << "ns/op |"
                  << std::setw(5) << implementationDiff.count() / 1000000 << "ms |"
                  << std::setw(4) << implementationDiff.count() / iterations  << "ns/op |" << std::endl
                  << script << std::endl
                  << std::endl;
    }
};

#endif //TLW_BENCHMARK_H
