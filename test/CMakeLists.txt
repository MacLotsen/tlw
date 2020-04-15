# Typed Lua Wrapping
# Copyright (C) 2019  Erik Nijenhuis
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

find_package(GTest)

set(SOURCE_FILES main.cpp
        detail/inspector_test.cpp
        detail/type_traits_test.cpp
        detail/stack_test.cpp
        user/constructor_test.cpp
        user/move_value_test.cpp
        user/property_test.cpp high_end/trivial_methods_test.cpp high_end/overloaded_ctor_test.cpp high_end/any_test.cpp high_end/source_code_test.cpp high_end/overloaded_methods_test.cpp)

add_executable(tlw-tests ${SOURCE_FILES})
target_link_libraries(tlw-tests ${GTEST_LIBRARY} pthread tlw)
target_include_directories(tlw-tests PRIVATE ${GTEST_INCLUDE_DIR})
add_test(NAME TestAll COMMAND ./tlw-tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
