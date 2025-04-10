//
// Created by jfbil on 2023-06-04.
//

#include <utility>

#include "nomad/script/Script.hpp"

namespace nomad {
Script::Script(
    NomadId      id,
    NomadString  name,
    NomadString  path,
    NomadString  source
):
    m_id(id),
    m_name(std::move(name)),
    m_path(std::move(path)),
    m_source(std::move(source))
{
}

Script::~Script() = default;


void Script::addParameter(const NomadString& parameterName, const Type* type) {
    m_parameters.registerVariable(parameterName, type);
}

NomadId Script::getParameterId(const NomadString& parameterName) const {
    return m_parameters.getVariableId(parameterName);
}

const NomadString& Script::getParameterName(NomadId parameterId) const {
    return m_parameters.getVariableName(parameterId);
}

const Type* Script::getParameterType(NomadId parameterId) const {
    return m_parameters.getVariableType(parameterId);
}

NomadIndex Script::getParameterCount() const {
    return m_parameters.getVariableCount();
}


NomadId Script::registerVariable(const NomadString& variableName, const Type* type) {
    auto variableId = m_variables.getVariableId(variableName);

    if (variableId != NOMAD_INVALID_ID) {
        auto currentType = m_variables.getVariableType(variableId);

        if (currentType == nullptr) {
            m_variables.setVariableType(variableId, type);
        } else if (currentType != type) {
            throw NomadException("Internal error: Variable '" + variableName + "' already exists with a different type");
        }

        return variableId;
    }

    return m_variables.registerVariable(variableName, type);
}

NomadId Script::getVariableId(const NomadString& variableName) const {
    return m_variables.getVariableId(variableName);
}

[[nodiscard]] const NomadString& Script::getVariableName(NomadId variableId) const {
    return m_variables.getVariableName(variableId);
}

void Script::setVariableType(NomadId variableId, const Type* type) {
    m_variables.setVariableType(variableId, type);
}

const Type* Script::getVariableType(NomadId variableId) const {
    return m_variables.getVariableType(variableId);
}

NomadIndex Script::getVariableCount() const {
    return m_variables.getVariableCount();
}

void Script::setReturnType(const Type* returnType) {
    m_returnType = returnType;
}

const Type* Script::getReturnType() const {
    return m_returnType;
}

void Script::setScriptStart(NomadIndex scriptStartIndex) {
    m_scriptStartIndex = scriptStartIndex;
}

NomadIndex Script::getScriptStart() const {
    return m_scriptStartIndex;
}

void Script::setScriptEnd(NomadIndex scriptEndIndex) {
    m_scriptEndIndex = scriptEndIndex;
}

NomadIndex Script::getScriptEnd() const {
    return m_scriptEndIndex;
}

NomadIndex Script::getScriptLength() const {
    return m_scriptEndIndex - m_scriptStartIndex;
}

} // nomad