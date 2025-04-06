//
// Created by jfbil on 2023-06-05.
//

#include <iomanip>
#include <sstream>

#include "nomad/system/String.hpp"

namespace nomad {
NomadString toString(NomadBoolean value) {
    return value ? "true" : "false";
}

NomadString toString(NomadId value) {
    return std::to_string(value);
}

NomadString toString(NomadInteger value) {
    return std::to_string(value);
}

NomadString toString(NomadFloat value) {
    std::stringstream ss;

    ss << std::setprecision(15) << std::noshowpoint << value;

    return ss.str();
}

NomadString toString(NomadIndex index) {
    return std::to_string(index);
}

void split(const TempString& text, const NomadString& separator, TempStringVector& lines) {
    size_t start = 0;
    size_t end = text.find(separator);

    while (end != NomadString::npos) {
        lines.push_back(text.substr(start, end - start));
        start = end + separator.length();
        end = text.find(separator, start);
    }

    lines.push_back(text.substr(start));
}

void splitLines(const NomadString &text, std::vector<NomadString> &lines) {
    size_t start = 0;
    size_t end = text.find_first_of("\r\n");

    while (end != NomadString::npos) {
        lines.push_back(text.substr(start, end - start));
        if (text[end] == '\r' && end + 1 < text.size() && text[end + 1] == '\n') {
            start = end + 2;
        } else {
            start = end + 1;
        }
        end = text.find_first_of("\r\n", start);
    }

    lines.push_back(text.substr(start));
}

void splitLines(const TempString &text, TempStringVector &lines) {
    size_t start = 0;
    size_t end = text.find_first_of("\r\n");

    while (end != NomadString::npos) {
        lines.push_back(text.substr(start, end - start));
        if (text[end] == '\r' && end + 1 < text.size() && text[end + 1] == '\n') {
            start = end + 2;
        } else {
            start = end + 1;
        }
        end = text.find_first_of("\r\n", start);
    }

    lines.push_back(text.substr(start));
}

} // nomad
