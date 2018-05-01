add_library(synch
    coarse_grain_list.h
    dllist.h
    dlnode.h
    fine_grain_list.h
    hashmap.h
    list_runner.h
    list_runner.cpp
    lockfree_list.h
    nonblocking_list.h
    util.h
    util.cpp
)
add_executable(lb lb.cpp)
target_link_libraries(lb synch ${CMAKE_THREAD_LIBS_INIT})

add_executable(lftest lftest.cpp)
target_link_libraries(lftest synch ${CMAKE_THREAD_LIBS_INIT})
