//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_PROP_EXAMPLE_H
#define TLW_PROP_EXAMPLE_H

struct PropertyExample {
    inline static const char * script = "local oldPropertyValue = example.property\n"
                                        "example.property = 'property changed'\n"
                                        "return oldPropertyValue";

    std::string property;
};

#endif //TLW_PROP_EXAMPLE_H
