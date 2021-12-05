#include "BufferAllocator.hpp"

#include <cassert>

namespace Video {

BufferAllocator::BufferAllocator(uint32_t frames) {
    this->frames = frames;

    freeBufferObjects = new std::vector<Buffer*>[frames];
    usedBufferObjects = new std::vector<Buffer*>[frames];

    pools = new Pool[frames];
    for (uint32_t i = 0; i < frames; ++i) {
        for (uint32_t usage = 0; usage < static_cast<uint32_t>(Buffer::BufferUsage::COUNT); ++usage) {
            pools[frame].currentRawBuffer[usage] = 0;
            pools[frame].currentOffset[usage] = 0;
        }
    }
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
            for (RawBuffer* rawBuffer : pools[i].rawBuffers[usage]) {
                delete rawBuffer;
            }
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
        pools[frame].currentRawBuffer[usage] = 0;
        pools[frame].currentOffset[usage] = 0;
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

    // Use next buffer if we've run out of space in the current one.
    uint32_t bufferUsageInt = static_cast<uint32_t>(bufferUsage);
    if (pools[frame].currentOffset[bufferUsageInt] + size > poolSize) {
        pools[frame].currentRawBuffer[bufferUsageInt]++;
        pools[frame].currentOffset[bufferUsageInt] = 0;
    }

    // Get a new allocation if we don't have any more buffers to suballocate from.
    if (pools[frame].rawBuffers[bufferUsageInt].size() <= pools[frame].currentRawBuffer[bufferUsageInt]) {
        pools[frame].rawBuffers[bufferUsageInt].push_back(Allocate(bufferUsage, true, poolSize));
    }

    // Suballocate.
    BufferAllocation allocation;
    allocation.buffer = pools[frame].rawBuffers[bufferUsageInt][pools[frame].currentRawBuffer[bufferUsageInt]];
    allocation.offset = pools[frame].currentOffset[bufferUsageInt];
    allocation.size = size;
    allocation.temporaryAllocation = true;

    pools[frame].currentOffset[bufferUsageInt] += size;

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
