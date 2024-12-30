//
// Created by jfbil on 2024-12-22.
//

#ifndef NOMAD_FASTHEAP_HPP
#define NOMAD_FASTHEAP_HPP

#include "nomad/Nomad.hpp"

#include <string>
#include <vector>

namespace nomad {

void* fast_temp_heap_alloc(std::size_t size);
void fast_temp_heap_free(void* ptr); // This is a no-op
[[nodiscard]] std::size_t fast_temp_heap_size();
void fast_temp_heap_reset();

template<typename T>
struct FastTempAllocator {
    using value_type = T;

    FastTempAllocator() noexcept = default;

    template<typename U>
    constexpr explicit FastTempAllocator(const FastTempAllocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) {
        return static_cast<T*>(fast_temp_heap_alloc(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept {
        // Nothing to do. Memory is freed when the heap is reset.
    }
};

class TempString: public std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>> {
public:
    TempString() = default;
    TempString(const NomadString& string) : std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>>(string) {}
    TempString(const NomadChar* string) : std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>>(string) {}
    TempString(const TempString& other) = default;

    [[nodiscard]] const NomadString& to_nomad_string_ref() const {
        return *reinterpret_cast<const NomadString*>(this);
    }

    [[nodiscard]] NomadString to_nomad_string() const {
        return NomadString(*this);
    }
};

template<typename T>
class TempVector: public std::vector<T, FastTempAllocator<T>> {
public:
    TempVector() = default;
    TempVector(const std::vector<T>& vector) : std::vector<T, FastTempAllocator<T>>(vector) {}
    TempVector(const TempVector& other) = default;

    [[nodiscard]] const std::vector<T>& to_std_vector_ref() const {
        return *reinterpret_cast<const std::vector<T>*>(this);
    }

    [[nodiscard]] std::vector<T> to_std_vector() const {
        return std::vector<T>(*this);
    }
};

using TempStringStream = std::basic_stringstream<char, std::char_traits<char>, FastTempAllocator<char>>;


} // namespace nomad

#endif //NOMAD_FASTHEAP_HPP
