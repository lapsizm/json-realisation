cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)

include("cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.314.tar.gz"
        SHA1 "95c47c92f68edb091b5d6d18924baabe02a6962a"
)

project(json VERSION 0.1.0)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)


add_executable(demo
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )


if(BUILD_TESTS)
    add_executable(tests
            ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
            )
    target_link_libraries(tests GTest::gtest_main)
    enable_testing()
    add_test(NAME unit_tests COMMAND tests)
endif()

