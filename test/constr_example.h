//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_CONSTR_EXAMPLE_H
#define TLW_CONSTR_EXAMPLE_H

#include <tlw/wrapping.hpp>

class ConstructorExample {
public:
    inline static std::vector<ConstructorExample *> examples;
    inline static const char * script = "local e1 = ConstructorExample()\n"
                                        "local e2 = ConstructorExample()\n"
                                        "local e3 = ConstructorExample()\n"
                                        "local e4 = ConstructorExample()\n"
                                        "local e5 = ConstructorExample()";

    ConstructorExample() {
        examples.push_back(this);
    }

    inline static ConstructorExample *create() {
        return new ConstructorExample;
    }
};

#endif //TLW_CONSTR_EXAMPLE_H
