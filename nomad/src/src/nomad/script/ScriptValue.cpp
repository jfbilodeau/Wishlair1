//
// Created by jbilodeau on 2024-09-17.
//

#include "nomad/script/ScriptValue.hpp"

#include <cstring>

namespace nomad {

ScriptValue::ScriptValue() {
    m_integer_value = 0;
}

ScriptValue::ScriptValue(const ScriptValue& other) {
    m_integer_value = other.m_integer_value;
}

ScriptValue::ScriptValue(NomadIndex value) {
    m_index_value = value;
}

ScriptValue::ScriptValue(NomadFloat value) {
    m_float_value = value;
}

ScriptValue::ScriptValue(NomadInteger value) {
    m_integer_value = value;
}

ScriptValue::ScriptValue(NomadBoolean value) {
    m_boolean_value = value;
}

ScriptValue::ScriptValue(NomadId value) {
    m_id_value = value;
}

ScriptValue::ScriptValue(const NomadString& value) {
    m_string_value = nullptr;
    set_string_value(value);
}

void ScriptValue::set_float_value(NomadFloat value) {
    m_float_value = value;
}

void ScriptValue::set_integer_value(NomadInteger value) {
    m_integer_value = value;
}

void ScriptValue::set_index_value(NomadIndex value) {
    m_index_value = value;
}

void ScriptValue::set_boolean_value(NomadBoolean value) {
    m_boolean_value = value;
}

void ScriptValue::set_string_value(const NomadString& value) {
    set_string_value(value.c_str());
}

void ScriptValue::set_id_value(NomadId value) {
    m_id_value = value;
}

void ScriptValue::set_string_value(const NomadChar* value) {
    if (value == nullptr) {
        m_string_value = nullptr;
        return;
    }

    const auto length = std::strlen(value);
    m_string_value = new char[length + 1];
    std::copy(
        value,
        value + length + 1,  // +1 to copy the null terminator
        m_string_value
    );
}

void ScriptValue::set(NomadFloat value) {
    set_float_value(value);
}

void ScriptValue::set(NomadInteger value) {
    set_integer_value(value);
}

void ScriptValue::set(NomadBoolean value) {
    set_boolean_value(value);
}

void ScriptValue::set(const NomadChar* value) {
    set_string_value(value);
}

NomadFloat ScriptValue::get_float_value() const {
    return m_float_value;
}

NomadInteger ScriptValue::get_integer_value() const {
    return m_integer_value;
}

NomadIndex ScriptValue::get_index_value() const {
    return m_index_value;
}

NomadBoolean ScriptValue::get_boolean_value() const {
    return m_boolean_value;
}

NomadId ScriptValue::get_id_value() const {
    return m_id_value;
}

const NomadChar* ScriptValue::get_string_value() const {
    return m_string_value;
}

void ScriptValue::init_string_value() {
    m_string_value = nullptr;
}

void ScriptValue::init_string_value(const NomadString& value) {
    set_string_value(value);
}

void ScriptValue::move_string_value(ScriptValue& other) {
    m_string_value = other.m_string_value;
#ifdef NOMAD_DEBUG
    other.m_string_value = nullptr;
#endif
}

void ScriptValue::free_string_value() const {
    delete m_string_value;
}

} // namespace nomad