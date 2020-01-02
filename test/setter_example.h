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
