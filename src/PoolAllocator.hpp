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

#pragma once

#include <cstdint>
#include <list>
#include "Chunk.hpp"

/**
 * \brief A pool allocator implementation
 *
 * Features:
 *
 *   - Parameterized by initial number of chunks per block, defaults to 128
 *   - Keeps track of the allocation pointer, blocks and total allocation size
 *   - Bump-allocates chunks
 *   - Requests a new block when needed, block size will increase * (1 << num_blocks) each time
 */
class PoolAllocator {
public:
    PoolAllocator() noexcept = default;
    explicit PoolAllocator(std::size_t chunks_per_block) noexcept : m_chunks_per_block(chunks_per_block) {}

    /**
     * \brief Returns the first free chunk in the block
     *
     * If there are no chunks left in the block,
     * allocates a new block.
     */
    auto allocate(std::size_t size) -> void*;

    /**
     * \brief Puts the chunk into the front of the chunks list
     */
    auto deallocate(void* ptr, std::size_t size) -> void;

    /**
     * \brief Get the total size of the pool
     */
    auto getPoolSize() const -> std::size_t;

    ~PoolAllocator();

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator(PoolAllocator&&) = delete;

    auto operator=(const PoolAllocator&) -> PoolAllocator& = delete;
    auto operator=(PoolAllocator&&) -> PoolAllocator = delete;

private:
    /**
     * Initial number of chunks per larger block.
     */
    std::size_t m_chunks_per_block{128};

    /**
     * Total allocation size
     */
    std::size_t m_total_size{0};

    /**
     * Track start of each block so we can free later
     */
    std::list<Chunk*> m_blocks{};

    /**
     * Pointer to the next free chunk
     */
    Chunk* m_allocation_ptr = nullptr;

    /**
     * \brief Allocates a new block from OS
     *
     * Returns a Chunk pointer set to the beginning of the block.
     */
    auto allocate_block(std::size_t chunk_size) -> Chunk*;
};
