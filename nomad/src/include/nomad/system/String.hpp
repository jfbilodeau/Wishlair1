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

namespace nomad {

class NomadStringBase {
public:
    NomadStringBase();
    NomadStringBase(const NomadStringBase& other);
    NomadStringBase(NomadStringBase&& other) noexcept;
    NomadStringBase(const NomadChar* cStr);
    NomadStringBase(const std::string& stdString);

    const NomadChar* toCString() const;
    const std::string& toStdString() const;
};

static NomadString stringConcatenate(const NomadString& a, const NomadString& b) {
    return a + b;
}

static NomadBoolean stringEqualTo(const NomadString& a, const NomadString& b) {
    return a == b;
}

static NomadBoolean stringEqualTo(const NomadChar* a, const NomadChar* b) {
    return std::strcmp(a, b) == 0;
}

static NomadBoolean stringNotEqualTo(const NomadString& a, const NomadString& b) {
    return a != b;
}

static NomadBoolean stringLessThan(const NomadString& a, const NomadString& b) {
    return a < b;
}

static NomadBoolean stringLessThanOrEqualTo(const NomadString& a, const NomadString& b) {
    return a <= b;
}

static NomadBoolean stringGreaterThan(const NomadString& a, const NomadString& b) {
    return a > b;
}

static NomadBoolean stringGreaterThanOrEqualTo(const NomadString& a, const NomadString& b) {
    return a >= b;
}

// trim from start (in place)
static NomadString& stringLeftTrim(NomadString &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    return s;
}

// trim from end (in place)
static NomadString& stringRightTrim(NomadString &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

// trim from both ends (in place)
static NomadString& stringTrim(NomadString &s) {
    stringRightTrim(s);
    stringLeftTrim(s);

    return s;
}

// trim from start (copying)
static NomadString stringLeftTrimCopy(NomadString s) {
    stringLeftTrim(s);
    return s;
}

// trim from end (copying)
static NomadString stringRightTrimCopy(NomadString s) {
    stringRightTrim(s);
    return s;
}

// trim from both ends (copying)
static NomadString stringTrimCopy(NomadString s) {
    stringTrim(s);
    return s;
}

NomadString toString(NomadBoolean value);
NomadString toString(NomadId value);
NomadString toString(NomadInteger value);
NomadString toString(NomadFloat value);
NomadString toString(NomadIndex index);

// Utility to split a string into a vector of strings at newline characters
void split(const TempString& text, const NomadString& separator, TempStringVector& lines);

void splitLines(const NomadString& text, std::vector<NomadString>& lines);
void splitLines(const TempString& text, TempStringVector& lines);

} // nomad

