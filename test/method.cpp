//
// Created by erik on 14-12-19.
//
#include "test.h"

class MethodExample {
private:
    unsigned int callMask = 0;
public:
    void method1() {
        callMask |= 1;
    }
    Number method2() {
        callMask |= 2;
        return 5.0;
    }
    Number method3(Number a, Number b) {
        callMask |= 4;
        return a + b;
    }
    void method4(Number, Number) {
        callMask |= 8;
    }
    unsigned int getCallMask() {
        return callMask;
    }
};

TEST(MethodTest, testMethods) {
    Lua _lua;

    ClassPrototype *classPrototype = ClassPrototypeBuilder("MethodExample")
            .method("method1", mk_method(&MethodExample::method1))
            .method("method2", mk_method(&MethodExample::method2))
            .method("method3", mk_method(&MethodExample::method3))
            .method("method4", mk_method(&MethodExample::method4))
            .build();

    MethodExample e;

    auto retValue = lua.add<MethodExample>(classPrototype)
            .file("m", "scripts/method.lua")
            .set("example", &e)
            .call("m");

    ASSERT_EQ(15, e.getCallMask());
    ASSERT_TRUE(retValue->is<Number>());
    ASSERT_EQ(10.0, retValue->to<Number>());
}
