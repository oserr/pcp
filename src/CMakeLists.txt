include_directories(../thirdparty/libcuckoo/install/include)

add_library(synch
    dlnode.h
    dllist.h
    coarse_grain_list.h
    fine_grain_list.h
    nonblocking_list.h
    lockfree_list.h
    hashmap.h
    libcuckoo_hashmap.h
    list_runner.h
    list_runner.cpp
    util.h
    util.cpp
)
add_executable(lb lb.cpp)
target_link_libraries(lb synch ${CMAKE_THREAD_LIBS_INIT})

add_executable(lftest lftest.cpp)
target_link_libraries(lftest synch ${CMAKE_THREAD_LIBS_INIT})
