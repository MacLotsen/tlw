cmake_minimum_required(VERSION 3.10)
project(SimpleLua VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(BUILD_TEST "Build GTest tests for Simple Lua" ON)

find_package(LuaJIT REQUIRED)

add_library(slua STATIC src/slua.cpp)
target_compile_features(slua PUBLIC cxx_std_17)
target_link_libraries(slua ${LUA_LIBRARY})
target_include_directories(slua
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${LUA_INCLUDE_DIR})

set_target_properties(slua PROPERTIES EXPORT_NAME SLUA)

install(TARGETS slua
        EXPORT slua-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(DIRECTORY include/
        DESTINATION include)

install(EXPORT slua-targets
        FILE SLuaTargets.cmake
        DESTINATION lib/cmake/SLUA)

set(CMAKE_CONFIG_DIR lib/cmake/SLUA)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/SLuaConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        cmake/SLuaConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/SLuaConfig.cmake
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
        ${CMAKE_CURRENT_BINARY_DIR}/SLuaConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SLuaConfigVersion.cmake
        DESTINATION ${CMAKE_CONFIG_DIR})

export(EXPORT slua-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/SLuaTargets.cmake)
export(PACKAGE SLUA)

if (BUILD_TEST)
    message(STATUS "Building tests")
    add_subdirectory(test)
endif ()
