
cmake_minimum_required(VERSION 3.28.0)

add_library(VeeCore)

set_target_properties(VeeCore PROPERTIES LINKER_LANGUAGE CXX)
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
)
target_include_directories(VeeCore PUBLIC Public/ PRIVATE Private/)
