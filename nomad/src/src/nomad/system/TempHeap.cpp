//
// Created by jfbil on 2024-12-22.
//

#include "nomad/system/TempHeap.hpp"

#include <array>
#include <vector>

namespace nomad {

namespace {
    std::array<std::byte, 1024 * 1024 * 10> buffer; // 10 MB
    auto temp_buffer = std::pmr::monotonic_buffer_resource(buffer.begin(), buffer.size());
}

std::pmr::monotonic_buffer_resource* getTempBuffer() {
    return &temp_buffer;
}

void resetTempHeap() {
    temp_buffer.release();
}

//
// namespace {
// std::byte fast_temp_heap[1024 * 1024 * 10]; // 10 MB
// std::size_t fast_temp_heap_index = 0;
// }
//
// void* fast_temp_heap_alloc(size_t size) {
//     auto alloc_index = fast_temp_heap_index;
//
//     // TODO: Check for overflow...
//
//     fast_temp_heap_index += size;
//
//     void* ptr = &fast_temp_heap[alloc_index];
//
//     return ptr;
// }
//
// void fast_temp_heap_free(void* ptr) {
//     // No op
// }
//
// std::size_t fast_temp_heap_size() {
//     return fast_temp_heap_index;
// }
//
// void fast_temp_heap_reset() {
//     fast_temp_heap_index = 0;
// }
//
// /////////////////////////////////////////////////////////////////////////////
// // TempAllocator
// class TempAllocator : public Allocator {
// public:
//     void* allocate(std::size_t size) override;
//
//     void deallocate(void* ptr) noexcept override;
// };
//
// void * TempAllocator::allocate(std::size_t size) {
//     return fast_temp_heap_alloc(size);
// }
//
// void TempAllocator::deallocate(void *ptr) noexcept {
//     // Nothing to do. Memory is freed when the heap is reset.
// }
//
// Allocator* get_temp_allocator() {
//     static TempAllocator temp_allocator;
//
//     return &temp_allocator;
// }

} // namespace nomad
