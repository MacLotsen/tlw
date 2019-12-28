//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_SETTER_EXAMPLE_H
#define TLW_SETTER_EXAMPLE_H

#include <string>

class SetterExample {
private:
    std::string property;
public:
    inline static const char * script = "example.property = 'property changed'";

    explicit SetterExample(std::string p) : property(std::move(p)) {}
    void setProperty(const std::string& p) { property = p; }
    std::string get() {
        return property;
    }
};

#endif //TLW_SETTER_EXAMPLE_H
