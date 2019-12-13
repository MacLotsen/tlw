cmake_minimum_required(VERSION 3.10)
project(SimpleLua)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(BUILD_TEST "Build GTest tests for Simple Lua" ON)

find_package(LuaJIT REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(include ${LUA_INCLUDE_DIR})
link_libraries(${LUA_LIBRARY})

if (BUILD_TEST)
    link_libraries(-lpthread)
    message(STATUS "Building tests")
    add_subdirectory(test)
endif (BUILD_TEST)
