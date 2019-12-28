//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_GETTER_EXAMPLE_H
#define TLW_GETTER_EXAMPLE_H

#include <tlw/wrapping.hpp>
#include <string>

class GetterExample {
private:
    std::string property;
public:
    inline static const char * script = "return example.property";
    explicit GetterExample(std::string p) : property(p) {}
    std::string getProperty() { return property; }
};

#endif //TLW_GETTER_EXAMPLE_H
