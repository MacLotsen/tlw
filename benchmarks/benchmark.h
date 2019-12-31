//
// Created by enijenhuis on 30-12-2019.
//

#ifndef TLW_BENCHMARK_H
#define TLW_BENCHMARK_H

#include <chrono>
#include <iostream>
#include <iomanip>
#include <utility>
#include <tlw/tlw.hpp>

class BenchmarkRunner {
public:
    virtual void prepare(const char *script) = 0;

    virtual void run() = 0;
};

class Benchmark {
    using clock = std::chrono::high_resolution_clock;
    using time = std::chrono::milliseconds;
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
            Benchmark(std::move(name), script, c, i, 1000000) {}

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

        double factor = classicDiff < implementationDiff
                        ? (double(implementationDiff.count()) / double(classicDiff.count()))
                        : (double(classicDiff.count()) / double(implementationDiff.count()));

        std::stringstream ss;
        ss << (classicDiff < implementationDiff ? "Classic = " : "Implementation = ")
           << std::setprecision(2) << ((factor - 1) * 100)
           << "% faster.";

        std::cout << std::setfill('_') << std::setw(80) << "" << std::endl << std::setfill(' ')
                  << std::left << "| " << std::setw(37) << name << "|"
                  << std::setw(19) << " Lua" << "|"
                  << std::setw(19) << " TLW" << "|" << std::endl
                  << "| " << std::setw(37) << ss.str() << "|"
                  << std::fixed << std::right
                  << std::setw(16) << classicDiff.count() << "ms |"
                  << std::setw(16) << implementationDiff.count() << "ms |" << std::endl
                  << script << std::endl
                  << std::endl;
    }
};

#endif //TLW_BENCHMARK_H
