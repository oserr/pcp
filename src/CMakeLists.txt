include_directories(
    ../thirdparty/libcuckoo/install/include
    ${TBB_INC}
)

add_library(synch
    benchmark_runner.h
    benchmark_runner.cpp
    dllist.h
    coarse_grain_list.h
    nonblocking_list.h
    lockfree_list.h
    hashmap.h
    libcuckoo_hashmap.h
    tbb_hashmap.h
    util.h
    util.cpp
)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark synch libtbb ${CMAKE_THREAD_LIBS_INIT})
