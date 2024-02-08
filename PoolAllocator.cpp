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

/**
 * Returns the first free chunk in the block.
 *
 * If there are no chunks left in the block,
 * allocates a new block.
 */
void *PoolAllocator::allocate(std::size_t size) {

    // No chunks left in the current block, or no any block
    // exists yet. Allocate a new one, passing the chunk size:

    if (mAlloc == nullptr) {
        mAlloc = allocateBlock(size);
    }

    // the allocation pointer:

    Chunk *freeChunk = mAlloc;

    // Advance (bump) the allocation pointer to the next chunk.
    //
    // When no chunks left, the `mAlloc` will be set to `nullptr`, and
    // this will cause allocation of a new block on the next request:

    mAlloc = mAlloc->next;

    return freeChunk;
}

/**
 * Allocates a new block from OS.
 *
 * Returns a Chunk pointer set to the beginning of the block.
 */
Chunk *PoolAllocator::allocateBlock(std::size_t chunkSize) {
    std::cout << "\nAllocating block (" << mChunksPerBlock << " chunks):\n\n";

    size_t blockSize = mChunksPerBlock * chunkSize;

    // The first chunk of the new block.
    auto *blockBegin = reinterpret_cast<Chunk *>(malloc(blockSize));
    TracyAlloc(blockBegin, sizeof(blockSize));

    // Once the block is allocated, we need to chain all
    // the chunks in this block:

    Chunk *chunk = blockBegin;

    for (int i = 0; i < mChunksPerBlock - 1; ++i) {
        chunk->next = reinterpret_cast<Chunk *>(reinterpret_cast<char *>(chunk) + chunkSize);
        chunk = chunk->next;
    }

    chunk->next = nullptr;

    return blockBegin;
}

/**
 * Puts the chunk into the front of the chunks list.
 */
void PoolAllocator::deallocate(void *chunk, size_t size) {

    // The freed chunk's next pointer points to the
    // current allocation pointer:

    reinterpret_cast<Chunk *>(chunk)->next = mAlloc;

    // And the allocation pointer is now set
    // to the returned (free) chunk:

    mAlloc = reinterpret_cast<Chunk *>(chunk);
}