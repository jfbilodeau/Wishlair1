//
// Created by jfbil on 2023-06-13.
//

#include "nomad/script/VariableContext.hpp"
#include "nomad/script/Variable.hpp"

#include "nomad/log/Logger.hpp"

namespace nomad {

SimpleVariableContext::SimpleVariableContext()
{ }

NomadId SimpleVariableContext::register_variable(const NomadString& name, const Type* type) {
    auto variable_id = get_variable_id(name);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = m_variables.register_variable(name, type);
        m_values.resize(m_variables.get_variable_count());
    } else if (m_variables.get_variable_type(variable_id) == nullptr) {
        m_variables.set_variable_type(variable_id, type);
    } else if (m_variables.get_variable_type(variable_id) != type) {
        log::warning("Context variable '" + name + "' already registered with a different type");
    }

    return variable_id;
}

const NomadString& SimpleVariableContext::get_variable_name(NomadId variable_id) const {
    return m_variables.get_variable_name(variable_id);
}

NomadId SimpleVariableContext::get_variable_id(const NomadString& name) const {
    return m_variables.get_variable_id(name);
}

void SimpleVariableContext::set_variable_type(NomadId variable_id, const Type* type) {
    m_variables.set_variable_type(variable_id, type);
}

const Type* SimpleVariableContext::get_variable_type(NomadId variable_id) const {
    return m_variables.get_variable_type(variable_id);
}

void SimpleVariableContext::set_value(NomadId variable_id, const ScriptValue& value) {
    m_values[variable_id] = value;
}

void SimpleVariableContext::get_value(NomadId variable_id, ScriptValue& value) {
    value = m_values[variable_id];
}

[[nodiscard]] const VariableMap* SimpleVariableContext::get_variable_map() const {
    return &m_variables;
}

} // nomad