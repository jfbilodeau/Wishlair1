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

void Type::free_value(ScriptValue& value) const {
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
    if (other == this) {
        return true;
    }

    if (other->get_name() == get_name()) {
        return true;
    }

    return false;
}

const ScriptType* Type::as_callback() const {
    return nullptr;
}

bool Type::is_void() const {
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// VoidType
NomadString VoidType::get_name() const {
    return VOID_TYPE_NAME;
}

void VoidType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    // Nothing to do...
}

void VoidType::to_string(const ScriptValue& value, NomadString& string) const {
    log::error("VoidType::to_string() called");
    string = "";
}

bool VoidType::is_void() const {
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// IdType
NomadString IdType::get_name() const {
    return ID_TYPE_NAME;
}

void IdType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_id_value(source_value.get_id_value());
}

void IdType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::to_string(value.get_id_value());
}

///////////////////////////////////////////////////////////////////////////////
// FloatType
NomadString FloatType::get_name() const {
    return FLOAT_TYPE_NAME;
}

void FloatType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_float_value(source_value.get_float_value());
}

void FloatType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::to_string(value.get_float_value());
}

///////////////////////////////////////////////////////////////////////////////
// IntegerType
NomadString IntegerType::get_name() const {
    return INTEGER_TYPE_NAME;
}

void IntegerType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_integer_value(source_value.get_integer_value());
}

void IntegerType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::to_string(value.get_integer_value());
}

///////////////////////////////////////////////////////////////////////////////
// BooleanType
NomadString BooleanType::get_name() const {
    return BOOLEAN_TYPE_NAME;
}

void BooleanType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_integer_value(source_value.get_integer_value());
}

void BooleanType::to_string(const ScriptValue& value, NomadString& string) const {
    auto boolean_value = value.get_boolean_value();

    string = nomad::to_string(boolean_value);
}

///////////////////////////////////////////////////////////////////////////////
// StringType
NomadString StringType::get_name() const {
    return STRING_TYPE_NAME;
}

void StringType::init_value(ScriptValue& value) const {
    value.set_string_value(nullptr);
}

void StringType::free_value(ScriptValue& value) const {
    delete[] value.get_string_value();
}

void StringType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_string_value(source_value.get_string_value());
}

void StringType::to_string(const ScriptValue& value, NomadString& string) const {
    string = value.get_string_value();
}

///////////////////////////////////////////////////////////////////////////////
// ScriptReferenceType
ScriptType::ScriptType(const std::vector<const Type*>& parameter_types, const Type* return_type):
    m_parameter_types(parameter_types),
    m_return_type(return_type)
{
    m_name = SCRIPT_TYPE_NAME + "(";

    for (auto parameter_type : parameter_types) {
        m_name += parameter_type->get_name();

        if (parameter_type != parameter_types.back()) {
            m_name += ", ";
        }
    }

    m_name += "):" + return_type->get_name();
}

NomadString ScriptType::get_name() const {
    return SCRIPT_TYPE_NAME;
}

void ScriptType::copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const {
    destination_value.set_id_value(source_value.get_id_value());
}

void ScriptType::to_string(const ScriptValue& value, NomadString& string) const {
    string = nomad::to_string(value.get_id_value());
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
    auto other_callback = other->as_callback();

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

const ScriptType* ScriptType::as_callback() const {
    return this;
}

} // namespace nomad