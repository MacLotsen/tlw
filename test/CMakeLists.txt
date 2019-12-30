find_package(GTest)

set(SOURCE_FILES main.cpp stack.cpp method_wrappers.cpp functional_wrappers.cpp)

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests ${GTEST_LIBRARY} pthread tlw)
target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIR})
add_test(NAME TestAll COMMAND ./tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
