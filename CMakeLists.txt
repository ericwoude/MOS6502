cmake_minimum_required(VERSION 3.14)

project(
    MOS6502
    DESCRIPTION "An emulator for the MOS Technology 6502."
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SOURCE_FILES src/cpu.cpp src/mem.cpp)

include_directories(include)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME})

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
    instruction_tests
    tests/load_tests.cpp
    tests/store_tests.cpp
    tests/register_transfer_tests.cpp
    tests/stack_operations_tests.cpp
)

target_link_libraries(
    instruction_tests
    gtest_main
    ${PROJECT_NAME}
)

include(GoogleTest)
gtest_discover_tests(instruction_tests)