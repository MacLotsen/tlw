//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_METHOD_EXAMPLE_H
#define TLW_METHOD_EXAMPLE_H

class MethodExample {
private:
    unsigned int callMask = 0;
public:
    inline static const char * script = "example.method1()\n"
                                        "local n1 = example.method2()\n"
                                        "local n2 = example.method3(2.5, 2.5)\n"
                                        "example.method4(0, 0)\n"
                                        "return n1 + n2";

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

#endif //TLW_METHOD_EXAMPLE_H
