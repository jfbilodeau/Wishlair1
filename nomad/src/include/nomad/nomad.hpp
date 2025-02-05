//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_HPP
#define NOMAD_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <string>

namespace nomad {

// Basic type definitions
using NomadBoolean = bool;
using NomadInteger = std::int64_t;
using NomadFloat = double;

constexpr NomadBoolean NOMAD_FALSE = false;
constexpr NomadBoolean NOMAD_TRUE = true;

constexpr NomadFloat NOMAD_PI = 3.14159265358979323846;

using NomadChar = char;
using NomadString = std::string;

const NomadString NOMAD_EMPTY_STRING;

using NomadShort = std::int16_t;

// An internal index type for Nomad objects
using NomadIndex = std::size_t;
constexpr NomadIndex NOMAD_INVALID_INDEX = std::numeric_limits<NomadIndex>::max()-1;

// An internal ID type for Nomad objects
using NomadId = std::int32_t;
constexpr NomadId NOMAD_INVALID_ID = -1;
constexpr NomadId NOMAD_ID_MIN = 0;
constexpr NomadId NOMAD_ID_MAX = std::numeric_limits<NomadId>::max()-1;

template<typename T>
NomadId to_nomad_id(T value) {
    return static_cast<NomadId>(value);
}

class NomadException : public std::exception {
public:
    explicit NomadException(NomadString message) : m_message(std::move(message)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return m_message.c_str();
    }

    [[nodiscard]] const NomadString& message() const {
        return m_message;
    }
private:
    NomadString m_message;
};

// Default values
constexpr NomadInteger NOMAD_DEFAULT_INTEGER = 0;
constexpr NomadFloat NOMAD_DEFAULT_FLOAT = 0.0;
constexpr NomadBoolean NOMAD_DEFAULT_BOOLEAN = false;
constexpr auto NOMAD_DEFAULT_STRING = "";

} // namespace nomad

#endif // NOMAD_HPP
