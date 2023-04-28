#include "BufferAllocator.hpp"

#include <cassert>

namespace Video {

BufferAllocator::BufferAllocator(uint32_t frames) {
    this->frames = frames;

    freeBufferObjects = new std::vector<Buffer*>[frames];
    usedBufferObjects = new std::vector<Buffer*>[frames];

    pools = new Pool[frames];
#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
    for (uint32_t i = 0; i < frames; ++i) {
        for (uint32_t usage = 0; usage < static_cast<uint32_t>(Buffer::BufferUsage::COUNT); ++usage) {
            pools[i].subPools[usage].currentRawBuffer = 0;
            pools[i].subPools[usage].currentOffset = 0;
        }
    }
#endif
}

BufferAllocator::~BufferAllocator() {
    for (uint32_t i = 0; i < frames; ++i) {
        for (Buffer* buffer : freeBufferObjects[i]) {
            delete buffer;
        }

        for (Buffer* buffer : usedBufferObjects[i]) {
            delete buffer;
        }

        for (uint32_t usage = 0; usage < static_cast<uint32_t>(Buffer::BufferUsage::COUNT); ++usage) {
#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
            for (RawBuffer* rawBuffer : pools[i].subPools[usage].rawBuffers) {
                delete rawBuffer;
            }
#elif HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_WHOLE
            for (auto& it : pools[i].subPools[usage]) {
                for (RawBuffer* rawBuffer : it.second.rawBuffers) {
                    delete rawBuffer;
                }
            }
#endif
        }
    }
    delete[] freeBufferObjects;
    delete[] usedBufferObjects;
    delete[] pools;
}

void BufferAllocator::BeginFrame() {
    frame = (frame + 1) % frames;

    for (Buffer* buffer : usedBufferObjects[frame]) {
        freeBufferObjects[frame].push_back(buffer);
    }
    usedBufferObjects[frame].clear();

    for (uint32_t usage = 0; usage < static_cast<uint32_t>(Buffer::BufferUsage::COUNT); ++usage) {
#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
        pools[frame].subPools[usage].currentRawBuffer = 0;
        pools[frame].subPools[usage].currentOffset = 0;
#elif HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_WHOLE
        for (auto& it : pools[frame].subPools[usage]) {
            it.second.currentRawBuffer = 0;
        }
#endif
    }
}

Buffer* BufferAllocator::CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    assert(size > 0);

    // Allocate a new buffer.
    BufferAllocation allocation;
    allocation.buffer = Allocate(bufferUsage, false, size);
    allocation.offset = 0;
    allocation.size = size;
    allocation.temporaryAllocation = false;

    // Write data.
    if (data != nullptr) {
        allocation.buffer->Write(allocation.offset, size, data);
    }

    // Create buffer object and return.
    return CreateBufferObject(bufferUsage, allocation);
}

Buffer* BufferAllocator::CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    assert(size > 0);

    uint32_t alignment = GetAlignment(bufferUsage);
    uint32_t allocationSize = ((size - 1) / alignment + 1) * alignment;
    BufferAllocation allocation = AllocateTemporary(bufferUsage, allocationSize);

    // Write data.
    if (data != nullptr) {
        allocation.buffer->Write(allocation.offset, size, data);
    }

    return CreateTemporaryBufferObject(bufferUsage, allocation);
}

BufferAllocation BufferAllocator::AllocateTemporary(Buffer::BufferUsage bufferUsage, unsigned int size) {
    /// @todo Handle large allocations.
    assert(size < poolSize);

    // Get the sub pool.
    uint32_t bufferUsageInt = static_cast<uint32_t>(bufferUsage);
#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
    SubPool& subPool = pools[frame].subPools[bufferUsageInt];
#elif HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_WHOLE
    SubPool& subPool = pools[frame].subPools[bufferUsageInt][size];
#endif

#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
    // Use next buffer if we've run out of space in the current one.
    if (subPool.currentOffset + size > poolSize) {
        subPool.currentRawBuffer++;
        subPool.currentOffset = 0;
    }
#endif

    // Get a new allocation if we don't have any more buffers to suballocate from.
    if (subPool.rawBuffers.size() <= subPool.currentRawBuffer) {
       subPool.rawBuffers.push_back(Allocate(bufferUsage, true, poolSize));
    }

    // Suballocate.
    BufferAllocation allocation;
    allocation.buffer = subPool.rawBuffers[subPool.currentRawBuffer];
    allocation.size = size;
    allocation.temporaryAllocation = true;

#if HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_SUBALLOCATE
    allocation.offset = subPool.currentOffset;
    subPool.currentOffset += size;
#elif HYMN_BUFFER_STRATEGY == HYMN_BUFFER_STRATEGY_WHOLE
    allocation.offset = 0;

    subPool.currentRawBuffer++;
#endif

    return allocation;
}

Buffer* BufferAllocator::CreateTemporaryBufferObject(Buffer::BufferUsage bufferUsage, const BufferAllocation& allocation) {
    Buffer* buffer;

    // Try to re-use old buffer object.
    if (!freeBufferObjects[frame].empty()) {
        buffer = freeBufferObjects[frame].back();
        freeBufferObjects[frame].pop_back();
        buffer->Reset(bufferUsage, allocation);
    } else {
        buffer = CreateBufferObject(bufferUsage, allocation);
    }

    // Add buffer to cache.
    usedBufferObjects[frame].push_back(buffer);

    return buffer;
}

}
