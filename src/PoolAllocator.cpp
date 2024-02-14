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
#include <cstdlib>
#include "PoolAllocator.hpp"

auto PoolAllocator::allocate(std::size_t size) -> void* {

    ZoneScopedNC("pool:allocate", 0x244787);

    // No chunks left in the current block, or no any block
    // exists yet. Allocate a new one, passing the chunk size:
    if (m_allocation_ptr == nullptr) {
        m_allocation_ptr = allocate_block(size);
    }

    // the allocation pointer:
    Chunk* free_chunk = m_allocation_ptr;

    assert(free_chunk != nullptr && "Can't return a null chunk");

    // Advance (bump) the allocation pointer to the next chunk.
    //
    // When no chunks left, the `m_allocation_ptr` will be set to `nullptr`, and
    // this will cause allocation of a new block on the next request:
    m_allocation_ptr = m_allocation_ptr->next;

    return free_chunk;
}

auto PoolAllocator::allocate_block(std::size_t chunk_size) -> Chunk* {

    ZoneScopedNC("pool:allocate_block", 0x244787);

    const std::size_t block_size{m_chunks_per_block * chunk_size};
    const std::size_t multiplier = static_cast<std::size_t>(1) << m_blocks.size();
    const std::size_t final_size{multiplier * block_size};

    // The first chunk of the new block.
    auto* block_begin = reinterpret_cast<Chunk*>(malloc(final_size));

    assert(block_begin != nullptr && "Got a null pointer for a block");

    TracyAllocN(block_begin, final_size, "pool");

    // Track total size
    m_total_size += final_size;

    // Add the block to the list
    m_blocks.push_back(block_begin);

    // Once the block is allocated, we need to chain all
    // the chunks in this block:
    Chunk* chunk = block_begin;
    for (int i = 0; i < (m_chunks_per_block * multiplier) - 1; ++i) {
        chunk->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunk_size);
        chunk = chunk->next;
    }

    // Mark the chunk as used
    chunk->next = nullptr;

    return block_begin;
}

auto PoolAllocator::deallocate(void* chunk, std::size_t /*size*/) -> void {

    ZoneScopedNC("pool:deallocate", 0x244787);

    // The freed chunk's next pointer points to the
    // current allocation pointer:
    reinterpret_cast<Chunk*>(chunk)->next = m_allocation_ptr;

    // And the allocation pointer is now set
    // to the returned (free) chunk:
    m_allocation_ptr = reinterpret_cast<Chunk*>(chunk);
}

auto PoolAllocator::getPoolSize() const -> std::size_t {
    return m_total_size;
}

PoolAllocator::~PoolAllocator() {

    ZoneScopedNC("pool:destructor", 0x244787);

    while(!m_blocks.empty()) {
        TracyFreeN(m_blocks.front(), "pool");
        delete m_blocks.front();
        m_blocks.pop_front();
    }
}
