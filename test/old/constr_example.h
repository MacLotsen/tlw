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

#ifndef TLW_CONSTR_EXAMPLE_H
#define TLW_CONSTR_EXAMPLE_H

#include <tlw/old/_wrapping.hpp>

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
