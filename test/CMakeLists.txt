include_directories(
    ../src
    ${GTEST_INC}
    ${GMOCK_INC}
)
# One executable for all unit tests.
add_executable(test_all
    test_dlnode.cpp
    test_dllist.cpp
    test_hashmap.cpp
    test_list.cpp
)
target_link_libraries(test_all libgtest)
