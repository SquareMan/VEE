
cmake_minimum_required(VERSION 3.28.0)

add_library(VeeCore)

target_link_libraries(VeeCore
        PRIVATE
        magic_enum
)

target_sources(VeeCore
        PUBLIC
        FILE_SET headers TYPE HEADERS
        BASE_DIRS Public/
        FILES
        Public/Assert.hpp
        Public/FNV-1a.hpp
        Public/Logging.hpp
        Public/Name.hpp
        Public/VeeCore.hpp

        PRIVATE
        FILE_SET private_headers TYPE HEADERS
        BASE_DIRS Private/
        FILES
        Private/LogUtil.hpp

        PRIVATE
        Private/Assert.cpp
        Private/Logging.cpp
        Private/Name.cpp
)
target_include_directories(VeeCore PUBLIC Public/ PRIVATE Private/)
