//
// Created by jfbil on 2024-12-22.
//

#ifndef NOMAD_FASTHEAP_HPP
#define NOMAD_FASTHEAP_HPP

#include "nomad/nomad.hpp"

#include "nomad/system/Memory.hpp"

#include <array>
#include <memory_resource>
#include <string>
#include <vector>

namespace nomad {

static std::array<std::byte, 1024 * 1024 * 10> buffer; // 10 MB

static auto fast_heap_allocator = std::pmr::monotonic_buffer_resource(buffer.begin(), buffer.size());

using TempString = std::basic_string<NomadChar, std::char_traits<NomadChar>, std::pmr::polymorphic_allocator<NomadChar>>;
using TempStringStream = std::basic_stringstream<char, std::char_traits<char>, std::pmr::polymorphic_allocator<char>>;
template<typename T>
using TempVector = std::vector<T, std::pmr::polymorphic_allocator<T>>;

inline void fast_temp_heap_reset() {
    fast_heap_allocator.release();
}

// using TempString = std::pmr::basic_string<NomadChar, std::char_traits<NomadChar>>;

//
// void* fast_temp_heap_alloc(std::size_t size);
// void fast_temp_heap_free(void* ptr); // This is a no-op
// [[nodiscard]] std::size_t fast_temp_heap_size();
// void fast_temp_heap_reset();
// Allocator* get_temp_allocator();
//
// template<typename T>
// struct FastTempAllocator {
//     using value_type = T;
//
//     FastTempAllocator() noexcept = default;
//
//     template<typename U>
//     constexpr explicit FastTempAllocator(const FastTempAllocator<U>&) noexcept {}
//
//     [[nodiscard]] T* allocate(std::size_t n) {
//         return static_cast<T*>(fast_temp_heap_alloc(n * sizeof(T)));
//     }
//
//     void deallocate(T* p, std::size_t n) noexcept {
//         // Nothing to do. Memory is freed when the heap is reset.
//     }
// };
//
// class TempString: public std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>> {
// public:
//     TempString() = default;
//     TempString(const NomadString& string) : std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>>(string) {}
//     TempString(const NomadChar* string) : std::basic_string<char, std::char_traits<char>, FastTempAllocator<char>>(string) {}
//     TempString(const TempString& other) = default;
//
//     [[nodiscard]] const NomadString& to_nomad_string_ref() const {
//         return *reinterpret_cast<const NomadString*>(this);
//     }
//
//     [[nodiscard]] NomadString to_nomad_string() const {
//         return NomadString(*this);
//     }
// };
//
// inline TempString operator+(const TempString& lhs, const TempString& rhs) {
//     TempString result(lhs);
//     result.append(rhs);
//
//     return result;
// }
//
// inline bool operator==(const TempString& lhs, const TempString& rhs) {
//     return lhs.to_nomad_string_ref() == rhs.to_nomad_string_ref();
// }
//
// inline bool operator!=(const TempString& lhs, const TempString& rhs) {
//     return lhs.to_nomad_string_ref() != rhs.to_nomad_string_ref();
// }
//
// template<typename T>
// class TempVector: public std::vector<T, FastTempAllocator<T>> {
// public:
//     TempVector() = default;
//     TempVector(const std::vector<T>& vector) : std::vector<T, FastTempAllocator<T>>(vector) {}
//     TempVector(const TempVector& other) = default;
//
//     [[nodiscard]] const std::vector<T>& to_std_vector_ref() const {
//         return *reinterpret_cast<const std::vector<T>*>(this);
//     }
//
//     [[nodiscard]] std::vector<T> to_std_vector() const {
//         return std::vector<T>(*this);
//     }
// };
//
// using TempStringStream = std::basic_stringstream<char, std::char_traits<char>, FastTempAllocator<char>>;
//

} // namespace nomad

#endif //NOMAD_FASTHEAP_HPP
