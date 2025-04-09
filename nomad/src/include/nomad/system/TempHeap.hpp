//
// Created by jfbil on 2024-12-22.
//

#pragma once

#include "nomad/nomad.hpp"

#include <memory_resource>
#include <string>
#include <vector>

namespace nomad {

using TempString = std::pmr::basic_string<NomadString::value_type, std::char_traits<NomadString::value_type>>;
template<typename T>
using TempVector = std::vector<T, std::pmr::polymorphic_allocator<T>>;
using TempStringVector = TempVector<TempString>;

std::pmr::monotonic_buffer_resource* getTempBuffer();

// Free all memory allocated in the temporary heap
void resetTempHeap();

template<typename T>
TempVector<T> createTempVector() {
    return TempVector<T>(getTempBuffer());
}

template<typename T>
TempVector<T> createTempVector(std::vector<T>& vector) {
    return TempVector<T>(vector.begin(), vector.end(), getTempBuffer());
}

template<typename T>
TempVector<T> createTempVector(const std::vector<T>& vector) {
    return TempVector<T>(vector.begin(), vector.end(), getTempBuffer());
}

inline TempString createTempString() {
    return TempString();
}

inline TempString createTempString(const NomadString& string) {
    return TempString(string, getTempBuffer());
}

inline TempString createTempString(const NomadChar* string) {
    return TempString(string, getTempBuffer());
}

inline TempStringVector createTempStringVector() {
    return TempStringVector(getTempBuffer());
}

inline TempStringVector createTempStringVector(const std::vector<NomadString>& strings) {
    return TempStringVector(strings.begin(), strings.end(), getTempBuffer());
}

} // namespace nomad

