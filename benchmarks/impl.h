//
// Created by enijenhuis on 30-12-2019.
//

#ifndef TLW_IMPL_H
#define TLW_IMPL_H

#include <tlw/types.hpp>
#include <tlw/wrapping.hpp>
#include "benchmark.h"

class ImplBenchmarkRunner : public BenchmarkRunner {
protected:
    LuaFunction<> *func = nullptr;
    Lua lua;
public:
    ~ImplBenchmarkRunner() {
        delete func;
    }

    void prepare(const char *script) override {
        func = new LuaFunction<>(lua.src<LuaFunction<>>(script));
    }

    void run() override {
        (*func)();
    }
};


class ImplPropertyBenchmark : public ImplBenchmarkRunner {
private:
    PropertyExample *example = nullptr;
public:
    ~ImplPropertyBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        lua.add<PropertyExample>(PrettyClassPrototypeBuilder("PropertyExample")
                                         .property("number", mk_property(&PropertyExample::number))
                                         .build());

        example = new PropertyExample;
        lua.global("example", example);
    }
};

class ImplGetterSetterBenchmark : public ImplBenchmarkRunner {
private:
    PropertyExample *example = nullptr;
public:
    ~ImplGetterSetterBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        lua.add<PropertyExample>(PrettyClassPrototypeBuilder("GetterSetterExample")
                                         .getter("number", mk_function(&PropertyExample::getNumber))
                                         .setter("number", mk_function(&PropertyExample::setNumber))
                                         .build());

        example = new PropertyExample;
        lua.global("example", example);
    }
};

class ImplGetterSetterFastBenchmark : public ImplBenchmarkRunner {
private:
    PropertyExample *example = nullptr;
public:
    ~ImplGetterSetterFastBenchmark() {
        delete example;
    }

    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        lua.add<PropertyExample>(ClassPrototypeBuilder("GetterSetterFastExample")
                                         .method("getNumber", mk_function(&PropertyExample::getNumber))
                                         .method("setNumber", mk_function(&PropertyExample::setNumber))
                                         .build());

        example = new PropertyExample;
        lua.global("example", example);
    }
};

class ImplScriptResultsBenchmark : public BenchmarkRunner {
protected:
    LuaFunction<std::tuple<double, bool, const char *>()> *func = nullptr;
    Lua lua;
public:
    ~ImplScriptResultsBenchmark() {
        delete func;
    }

    void prepare(const char *script) override {
        func = new LuaFunction<std::tuple<double, bool, const char *>()>(lua.src<LuaFunction<std::tuple<double, bool, const char *>()>>(script));
    }

    void run() override {
        auto r = (*func)();
    }
};

class ImplTableFetchBenchmark : public ImplBenchmarkRunner {
protected:
    LuaTable *table;
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
        table = new LuaTable(lua.global<LuaTable>("t"));
    }

    void run() override {
        auto num = table->get<double>("r");
    }
};

class ImplTableFetchesBenchmark : public ImplTableFetchBenchmark {
public:
    void run() override {
        auto num = table->all<double, double, double>("r", "g", "b");
    }
};

class ImplListFetchBenchmark : public ImplBenchmarkRunner {
protected:
    LuaList *table;
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
        table = new LuaList(lua.global<LuaList>("t"));
    }

    void run() override {
        auto num = table->get<double>(1);
    }
};

class ImplListFetchesBenchmark : public ImplListFetchBenchmark {
public:
    void run() override {
        auto num = table->all<double, double, double>(1, 2, 3);
    }
};

class ImplTableSetBenchmark : public ImplTableFetchBenchmark {
public:
    void run() override {
        table->set("key", 2.5);
    }
};

class ImplTableSetsBenchmark : public ImplTableFetchBenchmark {
public:
    void run() override {
        table->setAll(std::pair{"key1", 2.5}, std::pair{"key2", 2.5}, std::pair{"key3", 2.5});
    }
};

#endif //TLW_IMPL_H
