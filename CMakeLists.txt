cmake_minimum_required(VERSION 3.16)
project(COT5405_NetworkFlow_NPHard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_algorithms INTERFACE)
target_include_directories(project_algorithms INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(cot5405 src/main.cpp)
target_link_libraries(cot5405 PRIVATE project_algorithms)

add_executable(project_tests tests/test_algorithms.cpp)
target_link_libraries(project_tests PRIVATE project_algorithms)

add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark PRIVATE project_algorithms)

enable_testing()
add_test(NAME project_tests COMMAND project_tests)
