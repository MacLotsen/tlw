cmake_minimum_required(VERSION 3.10)
project(TLW)

execute_process(
        COMMAND git describe --tags --always
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE GIT_REV
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

message("Typed Lua Wrapping v${GIT_REV}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(BUILD_TEST "Build GTest tests for Simple Lua" ON)
option(BUILD_BENCHMARKS "Build Benchmarks in GTest" OFF)

find_package(LuaJIT REQUIRED)

add_library(tlw INTERFACE)
target_compile_features(tlw INTERFACE cxx_std_17)
target_link_libraries(tlw INTERFACE ${LUA_LIBRARY})
target_include_directories(tlw
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${LUA_INCLUDE_DIR})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(tlw INTERFACE STRICT_ARGUMENTS)
endif ()

set_target_properties(tlw PROPERTIES EXPORT_NAME TLW)

install(TARGETS tlw
        EXPORT tlw-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(DIRECTORY include/
        DESTINATION include)

install(EXPORT tlw-targets
        FILE TLWTargets.cmake
        DESTINATION lib/cmake/TLW)

set(CMAKE_CONFIG_DIR lib/cmake/TLW)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/TLWConfigVersion.cmake
        VERSION "${GIT_REV}"
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        cmake/TLWConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/TLWConfig.cmake
        INSTALL_DESTINATION ${CMAKE_CONFIG_DIR}
)

configure_file(
        "cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

install(FILES
        cmake/FindLuaJIT.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/TLWConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/TLWConfigVersion.cmake
        DESTINATION ${CMAKE_CONFIG_DIR})

export(EXPORT tlw-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/TLWTargets.cmake)
export(PACKAGE TLW)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()

if (BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()
