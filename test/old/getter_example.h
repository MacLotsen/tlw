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

#ifndef TLW_GETTER_EXAMPLE_H
#define TLW_GETTER_EXAMPLE_H

#include <utility>
#include <tlw/old/_wrapping.hpp>
#include <string>

class GetterExample {
private:
    std::string property;
public:
    inline static const char * script = "return example.property";
    explicit GetterExample(std::string p) : property(std::move(p)) {}
    std::string getProperty() { return property; }
};

#endif //TLW_GETTER_EXAMPLE_H
