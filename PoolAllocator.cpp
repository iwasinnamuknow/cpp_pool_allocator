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

#include "PoolAllocator.hpp"
#include <iostream>
#include <cassert>

/**
 * Returns the first free chunk in the block.
 *
 * If there are no chunks left in the block,
 * allocates a new block.
 */
void *PoolAllocator::allocate(std::size_t size) {

    // No chunks left in the current block, or no any block
    // exists yet. Allocate a new one, passing the chunk size:
    if (m_free_list.empty()) {
        m_blocks.push_back(allocateBlock(size));
    }

    {
        // the allocation pointer:
        auto* free_chunk = m_free_list.front();
        m_free_list.pop_front();

        // Advance (bump) the allocation pointer to the next chunk.
        //
        // When no chunks left, the `mAlloc` will be set to `nullptr`, and
        // this will cause allocation of a new block on the next request:

        return free_chunk;
    }
}

/**
 * Allocates a new block from OS.
 *
 * Returns a Chunk pointer set to the beginning of the block.
 */
void* PoolAllocator::allocateBlock(std::size_t chunkSize) {

    ZoneScopedN("PA:allocateBlock");

    if (m_chunk_size == 0) {
        m_chunk_size = chunkSize;
    } else {
        assert((m_chunk_size == chunkSize) && "Chunk size cannot vary");
    }

    //std::cout << "\nAllocating block (" << m_chunks_per_block << " chunks):\n\n";

    size_t expansion_modifier = (1 << m_blocks.size());
    assert(expansion_modifier > 0);
    size_t chunks = m_chunks_per_block * expansion_modifier;
    size_t blockSize = chunks * chunkSize;

    // The first chunk of the new block.
    auto *blockBegin = malloc(blockSize);

    TracyAllocN(blockBegin, blockSize, "PA_block");

    // Once the block is allocated, we need to chain all
    // the chunks in this block:
    for (int i = 0; i < chunks - 1; ++i) {
        m_free_list.push_back(reinterpret_cast<void *>(reinterpret_cast<char *>(blockBegin) + (chunkSize * i)));
    }

    return blockBegin;
}

/**
 * Puts the chunk into the front of the chunks list.
 */
void PoolAllocator::deallocate(void *chunk, size_t size) {
    ZoneScopedN("PA:deallocate");
    m_free_list.push_back(chunk);
}

PoolAllocator::~PoolAllocator() {

    // free each block and remove it from the allocation list
    for (auto block : m_blocks) {
        std::free(block);
        TracyFreeN(block, "PA_block");
    }

    m_blocks.clear();
    m_free_list.clear();
}

std::size_t PoolAllocator::getAllocatedSize() const {
    return m_chunks_per_block * m_chunk_size * m_blocks.size();
}
