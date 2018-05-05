include_directories(
    ../src
    ../thirdparty/libcuckoo/install/include
    ${GTEST_INC}
    ${GMOCK_INC}
    ${TBB_INC}
)
# One executable for all unit tests.
add_executable(test_all
    test_dlnode.cpp
    test_list.cpp
    test_async_list.cpp
    test_hashmap.cpp
    test_lockfree.cpp
)
target_link_libraries(test_all libgtest ${CMAKE_THREAD_LIBS_INIT} ${TBB_LIBS})

add_executable(test_lockfreelist test_lockfree.cpp)
target_link_libraries(test_lockfreelist libgtest)
