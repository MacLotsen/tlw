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

#ifndef TLW_CLASS_EXAMPLES_H
#define TLW_CLASS_EXAMPLES_H

struct ClassExample {
    double number;

    double get() const {
        return number;
    }
};

struct BaseExample {
    double number;

    double get() const {
        return number;
    }
};

struct DerivativeExample : public BaseExample {
    inline static const char * script = "n = example:number() -- expect property of base class\n"
                                        "example:print() -- expect print method to be standard like\n"
                                        "example:number(n + 2.5)";

    void printNumber() const {
        std::cout << number << std::endl;
    }
};

#endif //TLW_CLASS_EXAMPLES_H