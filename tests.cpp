//
// MIT Licence
//
// Copyright (c) 2024 Matt Harrison
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <tracy/Tracy.hpp>
#include <snitch/snitch.hpp>
#include <array>
#include "src/Object.hpp"

// Number of Objects to use in tests
constexpr std::size_t object_count{100};

// Instantiate our allocator, using 16 chunks per block
constexpr std::size_t chunks_per_block{16};
PoolAllocator Object::allocator{chunks_per_block};

TEST_CASE("Initial pool size is zero", "[pool_allocator]") {

    ZoneScopedNC("test:initial_pool_size", 0x13a134);

    // Do not initialize any objects yet
    std::array<Object*, object_count> objects{};

    // Pool size should be zero
    REQUIRE(Object::allocator.getPoolSize() == 0);
}

TEST_CASE("First allocation size", "[pool_allocator]") {

    ZoneScopedNC("test:small_pool_size", 0x13a134);

    // Do not initialize any objects yet
    std::array<Object*, object_count> objects{};

    // Initialize a single object
    objects[0] = new Object();

    // Validate pool size for first allocation
    REQUIRE(Object::allocator.getPoolSize() == sizeof(Object) * chunks_per_block);

    // Delete all the objects
    for (std::size_t index = 0; index < object_count; index++) {
        delete objects[index];
    }
}

TEST_CASE("Total allocation size", "[pool_allocator]") {

    ZoneScopedNC("test:larger_pool_size", 0x13a134);

    // Initialize all the objects
    std::array<Object*, object_count> objects{};
    for (std::size_t index = 0; index < object_count; index++) {
        objects[index] = new Object;
    }

    // Determine the correct pool size
    std::size_t base_size{sizeof(Object) * chunks_per_block};
    std::size_t alloc_size{base_size};
    std::size_t alloc_count{1};
    std::size_t object_total_size{sizeof(Object) * object_count};
    while(alloc_size < object_total_size) {
        auto increase = base_size * (static_cast<std::size_t>(1) << alloc_count);
        alloc_size += increase;
        alloc_count++;
    }

    // Validate pool size for larger allocation
    REQUIRE(Object::allocator.getPoolSize() == alloc_size);

    // Delete all the objects
    for (std::size_t index = 0; index < object_count; index++) {
        delete objects[index];
    }
}

TEST_CASE("Block reuse", "[pool_allocator]") {

    ZoneScopedNC("test:block_reuse", 0x13a134);

    // Initialize all the objects
    std::array<Object*, object_count> objects{};
    for (std::size_t index = 0; index < object_count; index++) {
        objects[index] = new Object;
    }

    std::size_t initial_size{Object::allocator.getPoolSize()};

    // Delete all the objects
    for (std::size_t index = 0; index < object_count; index++) {
        delete objects[index];
    }

    // Initialize all the objects again
    for (std::size_t index = 0; index < object_count; index++) {
        objects[index] = new Object;
    }

    // Check that no additional blocks were allocated
    REQUIRE(Object::allocator.getPoolSize() == initial_size);

    // Delete all the objects again...
    for (std::size_t index = 0; index < object_count; index++) {
        delete objects[index];
    }
}
