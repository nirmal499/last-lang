cmake_minimum_required(VERSION 3.22.1)
project(lang)

set(CMAKE_BUILD_TYPE Debug)

option(ENABLE_TESTING "Enable a Unit Testing Build" ON)

set(LIBRARY_NAME "lang_lib")
set(EXECUTABLE_NAME "executable")

add_subdirectory(lib)

# if (ENABLE_TESTING)
#	add_subdirectory(tests) # This tests source code will be linking to our library for testing
# endif()

add_subdirectory(lang)