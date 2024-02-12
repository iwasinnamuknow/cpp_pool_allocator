/*
 * MIT License

 * Copyright (c) 2024 Matt Harrison
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef POOL_ALLOCATOR_POOLALLOCATOR_HPP
#define POOL_ALLOCATOR_POOLALLOCATOR_HPP

#include <cstdint>
#include <list>
#include <tracy/Tracy.hpp>

/**
 * The allocator class.
 *
 * Features:
 *
 *   - Parametrized by number of chunks per block
 *   - Keeps track of the allocation pointer
 *   - Bump-allocates chunks
 *   - Requests a new larger block when needed
 *
 */
class PoolAllocator {
public:
    PoolAllocator() = default;
    ~PoolAllocator();

    void* allocate(std::size_t size);
    void deallocate(void *ptr, std::size_t size);

    std::size_t getAllocatedSize() const;

private:
    /**
     * Number of chunks per larger block.
     */
    std::size_t m_chunks_per_block{128};
    std::size_t m_chunk_size{0};

    std::list<void*> m_free_list{};
    std::list<void*> m_blocks{};

    /**
     * Allocates a larger block (pool) for chunks.
     */
    void* allocateBlock(std::size_t chunk_size);
};

#endif //POOL_ALLOCATOR_POOLALLOCATOR_HPP
