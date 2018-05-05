find_package(Threads REQUIRED)
# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    tbb
    URL https://github.com/wjakob/tbb/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/tbb
    # Disable install step
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(tbb source_dir binary_dir)

set(TBB_INC ${source_dir}/include)
set(TBB_LIBS
    ${binary_dir}/libtbb.so
    ${binary_dir}/libtbbmalloc.so
    ${binary_dir}/libtbbmalloc_proxy.so
)
