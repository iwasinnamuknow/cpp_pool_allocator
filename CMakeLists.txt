cmake_minimum_required(VERSION 3.28)

set(TRACY_NO_EXIT 1)

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

find_package(tracy CONFIG REQUIRED)

project(pool_allocator)

set(CMAKE_CXX_STANDARD 20)

add_executable(pool_allocator
        main.cpp
        src/PoolAllocator.cpp
)
target_include_directories(pool_allocator PUBLIC ${tracy_SOURCE_DIR}/public)
target_link_libraries(pool_allocator PUBLIC TracyClient)