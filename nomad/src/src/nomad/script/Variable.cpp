//
// Created by jfbil on 2023-06-07.
//

#include "nomad/log/Logger.hpp"

#include "nomad/script/Variable.hpp"

namespace nomad {

VariableDefinition::VariableDefinition(const NomadString& name, const Type* type):
    m_name(name),
    m_type(type)
{}

const NomadString& VariableDefinition::getName() const {
    return m_name;
}

void VariableDefinition::setType(const Type* type) {
    m_type = type;
}

const Type* VariableDefinition::getType() const {
    return m_type;
}

NomadId VariableMap::registerVariable(const NomadString& name, const Type* type) {
    auto variable_id = getVariableId(name);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = to_nomad_id(m_variables.size());
        m_variables.emplace_back(name, type);
    } else {
        auto existing_type = getVariableType(variable_id);

        if (existing_type == nullptr) {
            setVariableType(variable_id, type);
        } else if (type != nullptr && existing_type != type) {
            NomadString original_type = existing_type->getName();
            NomadString new_type = type->getName();

            log::error("[VariableMap::register_variable] Redefining variable `" + name + "` from `" + original_type + "` to `" + new_type + "`. Ignoring");
        }
    }

    return variable_id;
}

NomadId VariableMap::getVariableId(const NomadString& name) const {
    for (auto i = 0; i < m_variables.size(); ++i) {
        if (m_variables[i].getName() == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

const NomadString& VariableMap::getVariableName(NomadId variableId) const {
    return m_variables[variableId].getName();
}

void VariableMap::setVariableType(NomadId variableId, const Type* type) {
    m_variables[variableId].setType(type);
}

const Type* VariableMap::getVariableType(NomadId variableId) const {
    return m_variables[variableId].getType();
}

NomadIndex VariableMap::getVariableCount() const {
    return m_variables.size();
}

VariableList::VariableList(const VariableMap* variableMap):
    m_variableMap(*variableMap) {
    m_variableValues.resize(m_variableMap.getVariableCount());

    for (auto i = 0; i < m_variableMap.getVariableCount(); ++i) {
        NomadId variable_id = to_nomad_id(i);
        auto type = m_variableMap.getVariableType(variable_id);

        type->init_value(m_variableValues[variable_id]);
    }
}

VariableList::~VariableList() {
    for (auto i = 0; i < m_variableMap.getVariableCount(); ++i) {
        NomadId variable_id = to_nomad_id(i);

        auto type = m_variableMap.getVariableType(variable_id);

        type->freeValue(m_variableValues[variable_id]);
    }
}

NomadId VariableList::getVariableId(const NomadString& name) const {
    return m_variableMap.getVariableId(name);
}

const NomadString& VariableList::getVariableName(NomadId variableId) const {
    return m_variableMap.getVariableName(variableId);
}

const Type* VariableList::getVariableType(NomadId variableId) const {
    return m_variableMap.getVariableType(variableId);
}

NomadIndex VariableList::getVariableCount() const {
    return m_variableMap.getVariableCount();
}

void VariableList::setVariableValue(NomadId variableId, const ScriptValue& value) {
#ifdef NOMAD_DEBUG
    if (variableId >= m_variableValues.size()) {
        log::error("[VariableList::set_variable_value] Variable ID out of range");
    }
#endif

    m_variableValues[variableId] = value;
}

void VariableList::getVariableValue(NomadId variableId, ScriptValue& value) const {
#ifdef NOMAD_DEBUG
    if (variableId >= m_variableValues.size()) {
        log::error("[VariableList::get_script_variable_value] Variable ID out of range");

        return;
    }
#endif

    value = m_variableValues[variableId];
}

} // nomad