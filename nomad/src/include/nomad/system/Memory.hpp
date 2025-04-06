//
// Created by jfbil on 2025-02-02.
//

#pragma once

#include <cstddef>

namespace nomad {

class Allocator {
public:
    virtual ~Allocator() = default;

    [[nodiscard]] virtual void* allocate(std::size_t size) = 0;
    virtual void deallocate(void* ptr) noexcept = 0;
};

Allocator* getDefaultAllocator();

} // namespace nomad