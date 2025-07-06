
cmake_minimum_required(VERSION 3.28.0)

add_library(VeeCore)

target_sources(VeeCore
        PUBLIC
        FILE_SET headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        Public/Name.hpp
        Public/FNV-1a.hpp
        Public/VeeCore.hpp

        PRIVATE
        FILE_SET private_headers TYPE HEADERS
        BASE_DIRS Private/
        FILES

        PRIVATE
        Private/Name.cpp
)
target_include_directories(VeeCore PUBLIC Public/ PRIVATE Private/)
