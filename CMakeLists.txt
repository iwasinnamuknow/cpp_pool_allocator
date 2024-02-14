cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare (
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.9.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable (tracy)

FetchContent_Declare(snitch
        GIT_REPOSITORY https://github.com/snitch-org/snitch.git
        GIT_TAG        v1.2.0
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(snitch)

find_package(tracy CONFIG REQUIRED)
find_package(snitch CONFIG REQUIRED)

project(pool_allocator)

set(CMAKE_CXX_STANDARD 98)

add_executable(pool_allocator
        tests.cpp
        src/PoolAllocator.cpp
)
target_include_directories(pool_allocator SYSTEM PUBLIC
        ${tracy_SOURCE_DIR}/public
        ${snitch_SOURCE_DIR}/include
)
target_link_libraries(pool_allocator PUBLIC
        TracyClient
        snitch::snitch
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()