//
// Created by jbilodeau on 2024-09-17.
//

#include "nomad/script/Type.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/system/String.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Type
void Type::init_value(ScriptValue& value) const {
    // Default implementation does nothing
}

void Type::freeValue(ScriptValue& value) const {
    // Default implementation does nothing
}

bool Type::needs_init() const {
    return get_init_op_code() != NOMAD_INVALID_ID;
}

bool Type::needs_free() const {
    return get_free_op_code() != NOMAD_INVALID_ID;
}

bool Type::need_copy() const {
    return get_copy_op_code() != NOMAD_INVALID_ID;
}

NomadId Type::get_init_op_code() const {
    return NOMAD_INVALID_ID;
}

NomadId Type::get_free_op_code() const {
    return NOMAD_INVALID_ID;
}

NomadId Type::get_copy_op_code() const {
    return NOMAD_INVALID_ID;
}

NomadString Type::to_string(const ScriptValue& value) const {
    NomadString text_value;

    to_string(value, text_value);

    return text_value;
}

bool Type::same_type(const Type* other) const {
    if (other == nullptr) {
        return false;
    }

    if (other == this) {
        return true;
    }

    if (other->getName() == getName()) {
        return true;
    }

    return false;
}

const ScriptType* Type::asCallback() const {
    return nullptr;
}

bool Type::isVoid() const {
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// VoidType
NomadString VoidType::getName() const {
    return VOID_TYPE_NAME;
}

void VoidType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    // Nothing to do...
}

void VoidType::to_string(const ScriptValue& value, NomadString& string) const {
    log::error("VoidType::to_string() called");
    string = "";
}

bool VoidType::isVoid() const {
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// IdType
NomadString IdType::getName() const {
    return ID_TYPE_NAME;
}

void IdType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setIdValue(source_value.getIdValue());
}

void IdType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::toString(value.getIdValue());
}

///////////////////////////////////////////////////////////////////////////////
// FloatType
NomadString FloatType::getName() const {
    return FLOAT_TYPE_NAME;
}

void FloatType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setFloatValue(source_value.getFloatValue());
}

void FloatType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::toString(value.getFloatValue());
}

///////////////////////////////////////////////////////////////////////////////
// IntegerType
NomadString IntegerType::getName() const {
    return INTEGER_TYPE_NAME;
}

void IntegerType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setIntegerValue(source_value.getIntegerValue());
}

void IntegerType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::toString(value.getIntegerValue());
}

///////////////////////////////////////////////////////////////////////////////
// BooleanType
NomadString BooleanType::getName() const {
    return BOOLEAN_TYPE_NAME;
}

void BooleanType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setIntegerValue(source_value.getIntegerValue());
}

void BooleanType::to_string(const ScriptValue& value, NomadString& string) const {
    auto boolean_value = value.getBooleanValue();

    string = nomad::toString(boolean_value);
}

///////////////////////////////////////////////////////////////////////////////
// StringType
NomadString StringType::getName() const {
    return STRING_TYPE_NAME;
}

void StringType::init_value(ScriptValue& value) const {
    value.setStringValue(nullptr);
}

void StringType::freeValue(ScriptValue& value) const {
    delete[] value.getStringValue();
}

void StringType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setStringValue(source_value.getStringValue());
}

void StringType::to_string(const ScriptValue& value, NomadString& string) const {
    string = value.getStringValue();
}

///////////////////////////////////////////////////////////////////////////////
// ScriptReferenceType
ScriptType::ScriptType(const std::vector<const Type*>& parameter_types, const Type* return_type):
    m_parameter_types(parameter_types),
    m_return_type(return_type)
{
    m_name = SCRIPT_TYPE_NAME + "(";

    for (auto parameter_type : parameter_types) {
        m_name += parameter_type->getName();

        if (parameter_type != parameter_types.back()) {
            m_name += ", ";
        }
    }

    m_name += "):" + return_type->getName();
}

NomadString ScriptType::getName() const {
    return SCRIPT_TYPE_NAME;
}

void ScriptType::copyValue(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.setIdValue(source_value.getIdValue());
}

void ScriptType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::toString(value.getIdValue());
}

NomadIndex ScriptType::get_parameter_count() const {
    return m_parameter_types.size();
}

const Type* ScriptType::get_parameter_type(NomadIndex index) const {
    return m_parameter_types[index];
}

const Type* ScriptType::get_return_type() const {
    return m_return_type;
}

bool ScriptType::same_type(const Type* other) const {
    auto other_callback = other->asCallback();

    // Probably not necessary
    if (other_callback == nullptr) {
        return false;
    }

    if (other_callback->get_return_type() != get_return_type()) {
        return false;
    }

    if (other_callback->get_parameter_count() != get_parameter_count()) {
        return false;
    }

    for (auto i = 0; i < get_parameter_count(); ++i) {
        if (other_callback->get_parameter_type(i) != get_parameter_type(i)) {
            return false;
        }
    }

    return true;
}

const ScriptType* ScriptType::asCallback() const {
    return this;
}

} // namespace nomad