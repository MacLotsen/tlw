//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_DESTR_EXAMPLE_H
#define TLW_DESTR_EXAMPLE_H

class DestructorExample {
public:
    inline static bool destroyed = false;
    inline static const char * script = "example = nil\n"
                                        "collectgarbage()";

    DestructorExample() = default;

    static void destroy(DestructorExample *instance) {
        delete instance;
        destroyed = true;
    }
};

#endif //TLW_DESTR_EXAMPLE_H
