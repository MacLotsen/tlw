find_package(GTest)

set(SOURCE_FILES main.cpp scripts.cpp types.cpp functions.cpp class.cpp)

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests ${GTEST_LIBRARY})
target_include_directories(tests PRIVATE ${GTEST_INCLUDE_DIR})
add_test(NAME TestAll COMMAND ./tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

