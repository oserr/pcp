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
target_link_libraries(benchmark synch ${TBB_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable(tbb_hashmap tbb_hashmap.cpp)
target_link_libraries(tbb_hashmap ${TBB_LIBS} ${CMAKE_THREAD_LIBS_INIT})
