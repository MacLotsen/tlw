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
        user/property_test.cpp
        high_end/trivial_methods_test.cpp
        high_end/overloaded_ctor_test.cpp
        high_end/any_test.cpp
        high_end/source_code_test.cpp
        high_end/overloaded_methods_test.cpp
        high_end/inner_property_as_value_test.cpp
        high_end/operators_test.cpp user/cfunction_test.cpp
        high_end/table_iterator_test.cpp
        high_end/inner_table_test.cpp
        high_end/std_types_test.cpp)

# GTests
add_executable(tlw-tests ${SOURCE_FILES})
target_compile_options(tlw-tests PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(tlw-tests GTest::GTest GTest::Main pthread tlw)
target_include_directories(tlw-tests PRIVATE include ${GTEST_INCLUDE_DIR})

set(TEST_EXECUTABLE ./tlw-tests)
if ("${CMAKE_C_COMPILER}" STREQUAL "/usr/bin/x86_64-w64-mingw32-gcc-posix")
    set(TEST_EXECUTABLE ./tlw-tests.exe)
endif()
add_test(NAME TestAll COMMAND ${TEST_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(TEST_STD_EXECUTABLE ./tlw-std-tests)
if ("${CMAKE_C_COMPILER}" STREQUAL "/usr/bin/x86_64-w64-mingw32-gcc-posix")
    set(TEST_STD_EXECUTABLE ./tlw-std-tests.exe)
endif()
add_executable(tlw-std-tests high_end/std_types_test.cpp main.cpp)
target_compile_options(tlw-std-tests PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(tlw-std-tests GTest::GTest GTest::Main pthread tlw)
target_include_directories(tlw-std-tests PRIVATE include ${GTEST_INCLUDE_DIR})
add_test(NAME TestSTD COMMAND ${TEST_STD_EXECUTABLE} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Example Module
add_library(tlw-examplelib SHARED high_end/module.cpp)
target_compile_options(tlw-examplelib PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(tlw-examplelib tlw)
target_include_directories(tlw-examplelib PUBLIC include)

# Copy lua files to build dir
file(COPY ${CMAKE_SOURCE_DIR}/test/lua DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

