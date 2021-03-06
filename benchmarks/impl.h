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

#include <tlw/tlw.hpp>
#include "benchmark.h"

class ImplBenchmarkRunner : public BenchmarkRunner {
protected:
    tlw::function<void()> *func = nullptr;
    tlw::state L = tlw::state(lua_open());
    tlw::lua lua = tlw::lua(L);
public:    
    ~ImplBenchmarkRunner() {
        delete func;
    }

    void prepare(const char *script) override {
        func = new tlw::function<void()>(lua.src(script));
    }

    void run() override {
        (*func)();
    }
};


class ImplGlobalBooleanGetBenchmark : public ImplBenchmarkRunner {
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
    }

    void run() override {
        auto b = lua.get<bool>("b");
    }
};

class ImplGlobalBooleanSetBenchmark : public ImplBenchmarkRunner {
public:
    void run() override {
        lua.set("b", true);
    }
};

class ImplGlobalNumberGetBenchmark : public ImplBenchmarkRunner {
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
    }

    void run() override {
        auto n = lua.get<double>("n");
    }
};

class ImplGlobalNumberSetBenchmark : public ImplBenchmarkRunner {
public:
    void run() override {
        lua.set("n", 2.5);
    }
};

class ImplGlobalStringGetBenchmark : public ImplBenchmarkRunner {
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
    }

    void run() override {
        auto s = lua.get<const char *>("s");
    }
};

class ImplGlobalStringSetBenchmark : public ImplBenchmarkRunner {
public:
    void run() override {
        lua.set("s", "str");
    }
};

class ImplGlobalFunctionGetBenchmark : public ImplBenchmarkRunner {
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
    }

    void run() override {
        auto f = lua.get<tlw::function<bool()>>("f");
    }
};

class ImplGlobalFunctionSetBenchmark : public ImplBenchmarkRunner {
public:
    void run() override {
        lua.set("f", noop);
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

        tlw::define<PropertyExample>("PropertyExample")
                .prop("number", &PropertyExample::number)
                .build()(L);

        example = new PropertyExample;
        lua.set("example", example);
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
//        tlw::define<PropertyExample>("GetterSetterExample")
//                .getter("number", &PropertyExample::getNumber)
//                .setter("number", &PropertyExample::setNumber)
//                .build()(lua);

        example = new PropertyExample;
        lua.set("example", example);
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
//        ImplBenchmarkRunner::prepare(script);
//        lua.add<PropertyExample>(ClassPrototypeBuilder("GetterSetterFastExample")
//                                         .method("getNumber", mk_function(&PropertyExample::getNumber))
//                                         .method("setNumber", mk_function(&PropertyExample::setNumber))
//                                         .build());
//
//        example = new PropertyExample;
//        lua.setObject("example", example);
    }
};

class ImplScriptResultsBenchmark : public BenchmarkRunner {
protected:
    using fn_type = tlw::function<std::tuple<double, bool, const char *>()>;
    fn_type func = fn_type(tlw::reference());
    tlw::lua lua;
public:

    void prepare(const char *script) override {
        func = lua.src<tlw::function<std::tuple<double, bool, const char *>()>()>(script);
    }

    void run() override {
        auto r = func();
    }
};

class ImplTableFetchBenchmark : public ImplBenchmarkRunner {
protected:
    tlw::table *table;
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
        table = new tlw::table(lua.get<const char *, tlw::table>("t"));
    }

    void run() override {
        auto num = table->get<const char *, double>("r");
    }
};

class ImplTableFetchesBenchmark : public ImplTableFetchBenchmark {
public:
    void run() override {
        auto num = table->get_all<double, double, double>("r", "g", "b");
    }
};

class ImplListFetchBenchmark : public ImplBenchmarkRunner {
protected:
    tlw::table *table;
public:
    void prepare(const char *script) override {
        ImplBenchmarkRunner::prepare(script);
        ImplBenchmarkRunner::run();
        table = new tlw::table(lua.get<tlw::table>("t"));
    }

    void run() override {
        auto num = table->get<double>(1);
    }
};

class ImplListFetchesBenchmark : public ImplListFetchBenchmark {
public:
    void run() override {
        auto num = table->get_all<double, double, double>(1, 2, 3);
    }
};

class ImplTableSetBenchmark : public ImplTableFetchBenchmark {
public:
    void run() override {
        table->set("key", 2.5);
    }
};

//class ImplTableSetsBenchmark : public ImplTableFetchBenchmark {
//public:
//    void run() override {
//        table->setAll(std::pair{"key1", 2.5}, std::pair{"key2", 2.5}, std::pair{"key3", 2.5});
//    }
//};

#endif //TLW_IMPL_H
