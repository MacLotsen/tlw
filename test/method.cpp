//
// Created by erik on 14-12-19.
//
#include <tlw/wrapper.h>
#include <tlw/types.hpp>
#include "test.h"

class MethodExample {
private:
    unsigned int callMask = 0;
public:
    void method1() {
        callMask |= 1;
    }
    double method2() {
        callMask |= 2;
        return 5.0;
    }
    double method3(double a, double b) {
        callMask |= 4;
        return a + b;
    }
    void method4(double, double) {
        callMask |= 8;
    }
    unsigned int getCallMask() {
        return callMask;
    }
};

TEST(MethodTest, testMethods) {
    Lua _lua;

    PrettyClassPrototype *classPrototype = PrettyClassPrototypeBuilder("MethodExample")
            .method("method1", mk_method(&MethodExample::method1))
            .method("method2", mk_method(&MethodExample::method2))
            .method("method3", mk_method(&MethodExample::method3))
            .method("method4", mk_method(&MethodExample::method4))
            .build();

    MethodExample e;

    auto retValue = lua.add<MethodExample>(classPrototype)
            .global("example", &e)
            .file<LuaFunction<double()>>("test/scripts/method.lua")();

    ASSERT_EQ(15, e.getCallMask());
    ASSERT_EQ(10.0, retValue);
}
