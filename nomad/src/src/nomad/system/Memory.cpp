//
// Created by jfbil on 2025-02-04.
//

#include <cstdlib>

#include "nomad/system/Memory.hpp"

namespace nomad {

class HeapAllocator final : public Allocator {
public:
    void* allocate(std::size_t size) override {
        return std::malloc(size);
    }

    void deallocate(void* ptr) noexcept override {
        std::free(ptr);
    }
};


Allocator * get_default_allocator() {
    static HeapAllocator default_allocator;

    return &default_allocator;
}

} // namespace nomad