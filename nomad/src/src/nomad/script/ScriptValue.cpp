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
    setStringValue(value);
}

void ScriptValue::setFloatValue(NomadFloat value) {
    m_float_value = value;
}

void ScriptValue::setIntegerValue(NomadInteger value) {
    m_integer_value = value;
}

void ScriptValue::setIndexValue(NomadIndex value) {
    m_index_value = value;
}

void ScriptValue::setBooleanValue(NomadBoolean value) {
    m_boolean_value = value;
}

void ScriptValue::setStringValue(const NomadString& value) {
    setStringValue(value.c_str());
}

void ScriptValue::setIdValue(NomadId value) {
    m_id_value = value;
}

void ScriptValue::setStringValue(const NomadChar* value) {
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
    setFloatValue(value);
}

void ScriptValue::set(NomadInteger value) {
    setIntegerValue(value);
}

void ScriptValue::set(NomadBoolean value) {
    setBooleanValue(value);
}

void ScriptValue::set(const NomadChar* value) {
    setStringValue(value);
}

NomadFloat ScriptValue::getFloatValue() const {
    return m_float_value;
}

NomadInteger ScriptValue::getIntegerValue() const {
    return m_integer_value;
}

NomadIndex ScriptValue::get_index_value() const {
    return m_index_value;
}

NomadBoolean ScriptValue::getBooleanValue() const {
    return m_boolean_value;
}

NomadId ScriptValue::getIdValue() const {
    return m_id_value;
}

const NomadChar* ScriptValue::getStringValue() const {
    return m_string_value;
}

void ScriptValue::initStringValue() {
    m_string_value = nullptr;
}

void ScriptValue::initStringValue(const NomadString& value) {
    setStringValue(value);
}

void ScriptValue::move_string_value(ScriptValue& other) {
    m_string_value = other.m_string_value;
#ifdef NOMAD_DEBUG
    other.m_string_value = nullptr;
#endif
}

void ScriptValue::freeStringValue() const {
    delete m_string_value;
}

} // namespace nomad