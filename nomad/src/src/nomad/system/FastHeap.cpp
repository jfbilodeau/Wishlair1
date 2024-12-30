//
// Created by jfbil on 2024-12-22.
//

#include "nomad/system/FastHeap.hpp"

#include <vector>

namespace nomad {

namespace {
std::byte fast_temp_heap[1024 * 1024 * 10]; // 10 MB
std::size_t fast_temp_heap_index = 0;
}

void* fast_temp_heap_alloc(size_t size) {
    auto alloc_index = fast_temp_heap_index;

    fast_temp_heap_index += size;

    void* ptr = &fast_temp_heap[alloc_index];

    return ptr;
}

void fast_temp_heap_free(void* ptr) {
    // No op
}

std::size_t fast_temp_heap_size() {
    return fast_temp_heap_index;
}

void fast_temp_heap_reset() {
    fast_temp_heap_index = 0;
}

} // namespace nomad