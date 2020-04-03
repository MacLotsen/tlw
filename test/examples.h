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

#ifndef TLW_EXAMPLES_H
#define TLW_EXAMPLES_H

namespace tlw {

    struct example_tracker {
        static inline int custom_created = 0;
        static inline int created = 0;
        static inline int copied = 0;
        static inline int moved = 0;
        static inline int deleted = 0;

        static void clear() {
            custom_created = created = copied = moved = deleted = 0;
        }
    };

    struct example {
        static inline const float invalid = std::numeric_limits<float>::max();
        float val;

        explicit example() noexcept : val(invalid) {
            example_tracker::created++;
        }

        example(const example &e) : val(e.val) {
            example_tracker::copied++;
        }

        example(example &&e) noexcept : val(std::exchange(e.val, invalid)) {
            example_tracker::moved++;
        }

        explicit example(float val) : val(val) {
            example_tracker::custom_created++;
        }

        example& operator=(example &&e) noexcept {
            val = std::exchange(e.val, invalid);
            return *this;
        }

        example& operator=(const example &e) = default;

        virtual ~example() {
            example_tracker::deleted++;
        }

        explicit operator bool() const {
            return val != invalid;
        }

        void print() const {
            printf("The value is %f", val);
        }
    };

    struct example2 {
    };

    using lua_example_t = user_data_t<const example *>;
    using lua_invalid_example_t = user_data_t<const example2 *>;

}

#endif //TLW_EXAMPLES_H
