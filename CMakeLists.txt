# ----------------------------------------------------------------------------
# happyIndex
# ----------------------------------------------------------------------------

# Minimum cmake version
cmake_minimum_required (VERSION 3.14...3.20)
# Name of project and that it is C/C++.
project (
        kSpider
        LANGUAGES CXX C
        VERSION 2.0.0
)

set(default_build_type "Release")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++fs -fPIC -lgomp -lrt -fopenmp -Ofast")

include_directories("${PROJECT_SOURCE_DIR}/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/parallel-hashmap")

add_executable(brute_index bruteforce_index.cpp)
target_link_libraries(brute_index z)

# add_executable(set test_set.cpp)
# target_link_libraries(set z)