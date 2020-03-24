/*
 * Typed Lua Wrapping
 * Copyright (C) 2019  Erik Nijenhuis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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
        callMask |= 1u;
    }

    double method2() {
        callMask |= 2u;
        return 5.0;
    }

    double method3(double a, double b) {
        callMask |= 4u;
        return a + b;
    }

    void method4(double, double) {
        callMask |= 8u;
    }

    const MethodExample& getObject() const {
        return *this;
    }

    unsigned int getCallMask() const {
        return callMask;
    }
};

#endif //TLW_METHOD_EXAMPLE_H
