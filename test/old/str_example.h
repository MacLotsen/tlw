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
