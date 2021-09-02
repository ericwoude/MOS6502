cmake_minimum_required(VERSION 3.14)

project(
    cpp-template
    DESCRIPTION "A basic C++ project template with CMake and GoogleTest."
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

add_executable(
    cpp-template
    src/main.cpp
)
target_link_libraries(cpp-template)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    template_test
    tests/template_test.cpp
)
target_link_libraries(
    template_test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(template_test)