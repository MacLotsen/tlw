cmake_minimum_required(VERSION 3.14)
project(SimpleLua)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(BUILD_TEST "Build GTest tests for Simple Lua" ON)

find_package(LuaJIT REQUIRED)
find_package(GTest)

set(CMAKE_CXX_STANDARD 17)

include_directories(include ${LUA_INCLUDE_DIR})
link_libraries(${LUA_LIBRARY})

if (BUILD_TEST)
    message(STATUS "Building tests")
    add_subdirectory(test)
    target_link_libraries(tests ${GTEST_LIBRARY})
    target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIR})
endif (BUILD_TEST)
