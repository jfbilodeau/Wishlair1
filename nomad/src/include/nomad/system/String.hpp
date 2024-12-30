//
// Created by jfbil on 2023-06-05.
//

#include <algorithm>
#include <cctype>
#include <locale>

#include "nomad/Nomad.hpp"

#ifndef NOMAD_STRING_HPP
#define NOMAD_STRING_HPP

namespace nomad {

static inline NomadString string_concatenate(const NomadString& a, const NomadString& b) {
    return a + b;
}

static inline NomadBoolean string_equal_to(const NomadString& a, const NomadString& b) {
    return a == b;
}

static inline NomadBoolean string_equal_to(const NomadChar* a, const NomadChar* b) {
    return std::strcmp(a, b) == 0;
}

static inline NomadBoolean string_not_equal_to(const NomadString& a, const NomadString& b) {
    return a != b;
}

static inline NomadBoolean string_less_than(const NomadString& a, const NomadString& b) {
    return a < b;
}

static inline NomadBoolean string_less_than_or_equal_to(const NomadString& a, const NomadString& b) {
    return a <= b;
}

static inline NomadBoolean string_greater_than(const NomadString& a, const NomadString& b) {
    return a > b;
}

static inline NomadBoolean string_greater_than_or_equal_to(const NomadString& a, const NomadString& b) {
    return a >= b;
}

// trim from start (in place)
static inline NomadString& string_left_trim(NomadString &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    return s;
}

// trim from end (in place)
static inline NomadString& string_right_trim(NomadString &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

// trim from both ends (in place)
static inline NomadString& string_trim(NomadString &s) {
    string_right_trim(s);
    string_left_trim(s);

    return s;
}

// trim from start (copying)
static inline NomadString string_left_trim_copy(NomadString s) {
    string_left_trim(s);
    return s;
}

// trim from end (copying)
static inline NomadString string_right_trim_copy(NomadString s) {
    string_right_trim(s);
    return s;
}

// trim from both ends (copying)
static inline NomadString string_trim_copy(NomadString s) {
    string_trim(s);
    return s;
}

NomadString to_string(NomadBoolean value);
NomadString to_string(NomadId value);
NomadString to_string(NomadInteger value);
NomadString to_string(NomadFloat value);
NomadString to_string(NomadIndex index);

} // nomad

#endif // NOMAD_STRING_HPP
