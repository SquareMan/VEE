cmake_minimum_required(VERSION 3.28.0)

CPMAddPackage("gh:catchorg/Catch2@3.9.1")

add_executable(VeeCoreTests)
target_sources(VeeCoreTests
    PRIVATE
    Name.cpp
)

target_link_libraries(VeeCoreTests PRIVATE VeeCore Catch2::Catch2WithMain)

include(CTest)
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(VeeCoreTests)