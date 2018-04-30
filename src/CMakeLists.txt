add_library(synch
    coarse_grain_list.h
    dllist.h
    dlnode.h
    fine_grain_list.h
    hashmap.h
    list_runner.h
    list_runner.cpp
    nonblocking_list.h
    lockfree_list.h
)
add_executable(lb lb.cpp)
target_link_libraries(lb synch ${CMAKE_THREAD_LIBS_INIT})

#set(CMAKE_BUILD_TYPE Debug)

add_executable(lftest lftest.cpp)
target_link_libraries(lftest synch ${CMAKE_THREAD_LIBS_INIT})