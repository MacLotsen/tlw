//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_FUNC_EXAMPLE_H
#define TLW_FUNC_EXAMPLE_H

namespace FunctionExample {
    inline static const char *script = "noop();\n"
                                       "return plus(2, 3)";

    inline static double plus(double a, double b) {
        return a + b;
    }

    inline static void noop() {

    }

    inline static lua_CFunction lplus = mk_function(plus);
    inline static lua_CFunction lnoop = mk_function(noop);

}

#endif //TLW_FUNC_EXAMPLE_H
