include_directories(
    ../src
    ../thirdparty/libcuckoo/install/include
    ${GTEST_INC}
    ${GMOCK_INC}
    ${TBB_INC}
)
# One executable for all unit tests.
add_executable(test_all
    test_async_list.cpp
    test_dlnode.cpp
    test_hashmap.cpp
    test_list.cpp
    test_lockfree.cpp
    test_util.cpp
)
target_link_libraries(test_all
    synch
    ${CMAKE_THREAD_LIBS_INIT}
    libgtest
    libtbb
)
