//
// Created by enijenhuis on 31-12-2019.
//

#ifndef TLW_STR_EXAMPLE_H
#define TLW_STR_EXAMPLE_H
#include <string>

class StringExample {
private:
    std::string str;
public:
    inline static const char * script = "return example .. ' in Lua'";
    explicit StringExample(std::string s) : str(std::move(s)) {}
    const char * concat(const char *s) {
        return (str + s).data();
    }
};

#endif //TLW_STR_EXAMPLE_H
