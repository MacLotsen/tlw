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

    struct example {
        float val;

        explicit example() noexcept : val(5.5) {
            printf("example created\n");
        }

        example(const example &e) : val(e.val) {
            printf("example copied\n");
        }

        example(example &&e) noexcept : val(std::exchange(e.val, 0)) {
            printf("example moved\n");
        }

        explicit example(float val) : val(val) {
            printf("example created with %g\n", val);
        }

        example operator=(example &&e) noexcept {
            return example(std::move(e));
        }

        example operator=(const example &e) {
            return example(e);
        }

        virtual ~example() {
            printf("example destroyed\n");
        }
    };

    struct example2 {
    };

    using lua_example_t = user_data_t<const example *>;
    using lua_invalid_example_t = user_data_t<const example2 *>;

}

#endif //TLW_EXAMPLES_H
