add_library(synch
    benchmark_runner.h
    benchmark_runner.cpp
    coarse_grain_list.h
    dllist.h
    dlnode.h
    fine_grain_list.h
    hashmap.h
    lockfree_list.h
    nonblocking_list.h
    util.h
    util.cpp
)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark synch ${CMAKE_THREAD_LIBS_INIT})
