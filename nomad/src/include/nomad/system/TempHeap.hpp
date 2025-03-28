//
// Created by jfbil on 2024-12-22.
//

#pragma once

#include "nomad/nomad.hpp"

#include <memory_resource>
#include <sstream>
#include <string>
#include <vector>

namespace nomad {

using TempString = std::pmr::basic_string<NomadString::value_type, std::char_traits<NomadString::value_type>>;
template<typename T>
using TempVector = std::vector<T, std::pmr::polymorphic_allocator<T>>;
using TempStringVector = TempVector<TempString>;

std::pmr::monotonic_buffer_resource* get_temp_buffer();

void fast_temp_heap_reset();

template<typename T>
TempVector<T> create_temp_vector() {
    return TempVector<T>(get_temp_buffer());
}

template<typename T>
TempVector<T> create_temp_vector(std::vector<T>& vector) {
    return TempVector<T>(vector.begin(), vector.end(), get_temp_buffer());
}

template<typename T>
TempVector<T> create_temp_vector(const std::vector<T>& vector) {
    return TempVector<T>(vector.begin(), vector.end(), get_temp_buffer());
}

inline TempString create_temp_string() {
    return TempString();
}

inline TempString create_temp_string(const NomadString& string) {
    return TempString(string, get_temp_buffer());
}

inline TempString create_temp_string(const NomadChar* string) {
    return TempString(string, get_temp_buffer());
}

inline TempStringVector create_temp_string_vector() {
    return TempStringVector(get_temp_buffer());
}

inline TempStringVector create_temp_string_vector(const std::vector<NomadString>& strings) {
    return TempStringVector(strings.begin(), strings.end(), get_temp_buffer());
}

} // namespace nomad

