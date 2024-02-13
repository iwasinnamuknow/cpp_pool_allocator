cmake_minimum_required(VERSION 3.28)

set(TRACY_NO_EXIT 1)
set(TRACY_ENABLE 1)

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

set(CMAKE_CXX_STANDARD 20)

add_executable(pool_allocator
        tests.cpp
        src/PoolAllocator.cpp
)
target_include_directories(pool_allocator PUBLIC
        ${tracy_SOURCE_DIR}/public
        ${snitch_SOURCE_DIR}/include
)
target_link_libraries(pool_allocator PUBLIC
        TracyClient
        snitch::snitch
)