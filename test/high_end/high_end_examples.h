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

#ifndef TLW_HIGH_END_EXAMPLES_H
#define TLW_HIGH_END_EXAMPLES_H

#include <cmath>

namespace tlw {

    struct vec4 {
        float x;
        float y;
        float z;
        float w;

        vec4() : vec4(0) {}

        vec4(float n) : vec4(n, n, n) {}

        vec4(float x, float y, float z) : vec4(x, y, z, .0f) {}

        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        vec4(const vec4 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {
        }

        vec4(vec4 &&other) : x(std::exchange(other.x, 0)),
                             y(std::exchange(other.y, 0)),
                             z(std::exchange(other.z, 0)),
                             w(std::exchange(other.w, 0)) {
        }

        vec4 &operator=(const vec4 &other) = default;

        vec4 &operator=(vec4 &&other) noexcept {
            x = std::exchange(other.x, 0);
            y = std::exchange(other.y, 0);
            z = std::exchange(other.z, 0);
            w = std::exchange(other.w, 0);
            return *this;
        }

        vec4& addition(float factor) {
            x += factor;
            y += factor;
            z += factor;
            w += factor;
            return *this;
        }

        vec4& addition(const vec4&other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        static float length(const vec4 &v) {
            return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2) + std::pow(v.z, 2));
        }

        static bool invalid(const vec4 &v) {
            return length(v) == 0 && v.w == 0;
        }
    };

    struct mat4 {
        float x[4];
        float y[4];
        float z[4];
        float w[4];

        mat4() : x{0, 0, 0, 0},
                 y{0, 0, 0, 0},
                 z{0, 0, 0, 0},
                 w{0, 0, 0, 0} {}

        mat4(float scalar) : x{scalar, 0, 0, 0},
                             y{0, scalar, 0, 0},
                             z{0, 0, scalar, 0},
                             w{0, 0, 0, scalar} {}

        mat4(float x1, float x2, float x3, float x4,
             float y1, float y2, float y3, float y4,
             float z1, float z2, float z3, float z4,
             float w1, float w2, float w3, float w4) : x{x1, x2, x3, x4},
                                                       y{y1, y2, y3, y4},
                                                       z{z1, z2, z3, z4},
                                                       w{w1, w2, w3, w4} {}


        static bool invalid(const mat4 &m) {
            return m.w[3] == 0;
        }
    };

    struct entity {
        vec4 position;
        vec4 look_at;
        mat4 model;

        entity(vec4 position) : position(position), look_at(0, 0, -1), model() {

        }

        entity(mat4 model) : position(0, 0, 0, 1), look_at(0, 0, -1), model(model) {

        }
    };

}

#endif //TLW_HIGH_END_EXAMPLES_H
