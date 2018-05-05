cmake_minimum_required(VERSION 3.0.2)
project(pcp CXX)
find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS} -O3")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(googletest)
include(tbb)
add_subdirectory(src)
add_subdirectory(test)
