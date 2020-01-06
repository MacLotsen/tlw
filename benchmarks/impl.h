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
        func = new LuaFunction<>(lua.src(script));
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
        lua.setObject("example", example);
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
        lua.setObject("example", example);
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
        lua.setObject("example", example);
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
        func = new LuaFunction<std::tuple<double, bool, const char *>()>(
                lua.src<LuaFunction<std::tuple<double, bool, const char *>()>>(script));
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
        table = new LuaTable(lua.get<LuaTable>("t"));
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
        table = new LuaList(lua.get<LuaList>("t"));
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
