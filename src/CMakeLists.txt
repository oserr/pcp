add_library(synch
    coarse_grain_list.h
    dllist.h
    dlnode.h
    fine_grain_list.h
    hashmap.h
    list_runner.h
    list_runner.cpp
    nonblocking_list.h
)

add_executable(lb lb.cpp)
target_link_libraries(lb synch)
