//
// Created by enijenhuis on 24-12-2019.
//

#ifndef TLW_MATH_EXAMPLE_H
#define TLW_MATH_EXAMPLE_H
#include <cmath>

class NumberExample {
private:
    double myNumber;
public:
    static inline const char * script = "return -ne, ne + 1, ne - 1, ne * 2, ne / 1, ne % 2, ne ^ 2, ne ~= ne2, ne < ne2, ne <= ne2";

    NumberExample() : myNumber(1.0) {}
    explicit NumberExample(double n) : myNumber(n) {}

    /**
     * For complicated reasons the unary minus provides the userdata twice...
     * See (http://lua-users.org/lists/lua-l/2016-10/msg00351.html)
     *
     * @param self
     * @return
     */
    double inverse(NumberExample *) {
        return -myNumber;
    }

    double addition(double other) {
        return myNumber + other;
    }

    double subtraction(double other) {
        return myNumber - other;
    }

    double multiplication(double other) {
        return myNumber * other;
    }

    double division(double other) {
        return myNumber / other;
    }

    int modulo(int other) {
        return int(myNumber) % other;
    }

    double power(double other) {
        return std::pow(myNumber, other);
    }

    bool equality(NumberExample* other) {
        return myNumber == other->myNumber;
    }

    bool lessThan(NumberExample* other) {
        return myNumber < other->myNumber;
    }

    bool lessThanOrEqual(NumberExample* other) {
        return myNumber <= other->myNumber;
    }
};

#endif //TLW_MATH_EXAMPLE_H
