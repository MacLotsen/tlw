set(SOURCE_FILES main.cpp)

add_executable(tlw-benchmarks ${SOURCE_FILES})
target_link_libraries(tlw-benchmarks tlw)
