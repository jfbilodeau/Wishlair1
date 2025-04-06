//
// Created by jfbil on 2023-06-13.
//

#include "nomad/script/VariableContext.hpp"
#include "nomad/script/Variable.hpp"

#include "nomad/log/Logger.hpp"

namespace nomad {

SimpleVariableContext::SimpleVariableContext()
{ }

NomadId SimpleVariableContext::registerVariable(const NomadString& name, const Type* type) {
    auto variable_id = getVariableId(name);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = m_variables.registerVariable(name, type);
        m_values.resize(m_variables.getVariableCount());
    } else if (m_variables.getVariableType(variable_id) == nullptr) {
        m_variables.setVariableType(variable_id, type);
    } else if (m_variables.getVariableType(variable_id) != type) {
        log::warning("Context variable '" + name + "' already registered with a different type");
    }

    return variable_id;
}

const NomadString& SimpleVariableContext::getVariableName(NomadId variable_id) const {
    return m_variables.getVariableName(variable_id);
}

NomadId SimpleVariableContext::getVariableId(const NomadString& name) const {
    return m_variables.getVariableId(name);
}

void SimpleVariableContext::setVariableType(NomadId variable_id, const Type* type) {
    m_variables.setVariableType(variable_id, type);
}

const Type* SimpleVariableContext::getVariableType(NomadId variable_id) const {
    return m_variables.getVariableType(variable_id);
}

void SimpleVariableContext::setValue(NomadId variable_id, const ScriptValue& value) {
    m_values[variable_id] = value;
}

void SimpleVariableContext::getValue(NomadId variable_id, ScriptValue& value) {
    value = m_values[variable_id];
}

[[nodiscard]] const VariableMap* SimpleVariableContext::get_variable_map() const {
    return &m_variables;
}

} // nomad