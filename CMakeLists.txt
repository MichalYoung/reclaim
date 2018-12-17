cmake_minimum_required(VERSION 3.1...3.12)
project(AST
        DESCRIPTION "Reference counted storage)"
        LANGUAGES C CXX
        )
# Note inclusion of CXX above is a workaround for a CMake problem on
# MacOS X --- we are not using C++, but CMake doesn't see <stdlib.h>
# unless we specify it.

set(CMAKE_C_STANDARD 11)

# Example for working out details of refcount API
add_executable(BINTREE
        bintree.c bintree.h
        refcount.c refcount.h
        )

# The include-directories command is discouraged in
# modern CMake.  I need to learn how to set this up
# as a specific dependency for a target.
#
include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
