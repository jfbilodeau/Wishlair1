//
// Created by jfbil on 2023-06-05.
//

#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>
#include <vector>

#include "TempHeap.hpp"
#include "nomad/nomad.hpp"

#ifndef NOMAD_STRING_HPP
#define NOMAD_STRING_HPP

namespace nomad {

class NomadStringBase {
public:
    NomadStringBase();
    NomadStringBase(const NomadStringBase& other);
    NomadStringBase(NomadStringBase&& other) noexcept;
    NomadStringBase(const NomadChar* c_str);
    NomadStringBase(const std::string& std_string);

    const NomadChar* to_c_string() const;
    const std::string& to_std_string() const;
};

static NomadString string_concatenate(const NomadString& a, const NomadString& b) {
    return a + b;
}

static NomadBoolean string_equal_to(const NomadString& a, const NomadString& b) {
    return a == b;
}

static NomadBoolean string_equal_to(const NomadChar* a, const NomadChar* b) {
    return std::strcmp(a, b) == 0;
}

static NomadBoolean string_not_equal_to(const NomadString& a, const NomadString& b) {
    return a != b;
}

static NomadBoolean string_less_than(const NomadString& a, const NomadString& b) {
    return a < b;
}

static NomadBoolean string_less_than_or_equal_to(const NomadString& a, const NomadString& b) {
    return a <= b;
}

static NomadBoolean string_greater_than(const NomadString& a, const NomadString& b) {
    return a > b;
}

static NomadBoolean string_greater_than_or_equal_to(const NomadString& a, const NomadString& b) {
    return a >= b;
}

// trim from start (in place)
static NomadString& string_left_trim(NomadString &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    return s;
}

// trim from end (in place)
static NomadString& string_right_trim(NomadString &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

// trim from both ends (in place)
static NomadString& string_trim(NomadString &s) {
    string_right_trim(s);
    string_left_trim(s);

    return s;
}

// trim from start (copying)
static NomadString string_left_trim_copy(NomadString s) {
    string_left_trim(s);
    return s;
}

// trim from end (copying)
static NomadString string_right_trim_copy(NomadString s) {
    string_right_trim(s);
    return s;
}

// trim from both ends (copying)
static NomadString string_trim_copy(NomadString s) {
    string_trim(s);
    return s;
}

NomadString to_string(NomadBoolean value);
NomadString to_string(NomadId value);
NomadString to_string(NomadInteger value);
NomadString to_string(NomadFloat value);
NomadString to_string(NomadIndex index);

// Utility to split a string into a vector of strings at newline characters
void split(const TempString& text, const NomadString& separator, TempStringVector& lines);

void split_lines(const NomadString& text, std::vector<NomadString>& lines);
void split_lines(const TempString& text, TempStringVector& lines);

} // nomad

#endif // NOMAD_STRING_HPP
