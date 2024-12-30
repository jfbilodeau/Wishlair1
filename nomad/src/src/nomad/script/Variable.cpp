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

const NomadString& VariableDefinition::get_name() const {
    return m_name;
}

void VariableDefinition::set_type(const Type* type) {
    m_type = type;
}

const Type* VariableDefinition::get_type() const {
    return m_type;
}

NomadId VariableMap::register_variable(const NomadString& name, const Type* type) {
    auto variable_id = get_variable_id(name);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = to_nomad_id(m_variables.size());
        m_variables.emplace_back(name, type);
    }

    return variable_id;
}

NomadId VariableMap::get_variable_id(const NomadString& name) const {
    for (auto i = 0; i < m_variables.size(); ++i) {
        if (m_variables[i].get_name() == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

const NomadString& VariableMap::get_variable_name(NomadId variable_id) const {
    return m_variables[variable_id].get_name();
}

void VariableMap::set_variable_type(NomadId variable_id, const Type* type) {
    m_variables[variable_id].set_type(type);
}

const Type* VariableMap::get_variable_type(NomadId variable_id) const {
    return m_variables[variable_id].get_type();
}

NomadIndex VariableMap::get_variable_count() const {
    return m_variables.size();
}

VariableList::VariableList(const VariableMap* variable_map):
    m_variable_map(*variable_map) {
    m_variable_values.resize(m_variable_map.get_variable_count());

    for (auto i = 0; i < m_variable_map.get_variable_count(); ++i) {
        NomadId variable_id = to_nomad_id(i);
        auto type = m_variable_map.get_variable_type(variable_id);

        type->init_value(m_variable_values[variable_id]);
    }
}

VariableList::~VariableList() {
    for (auto i = 0; i < m_variable_map.get_variable_count(); ++i) {
        NomadId variable_id = to_nomad_id(i);

        auto type = m_variable_map.get_variable_type(variable_id);

        type->free_value(m_variable_values[variable_id]);
    }
}

NomadId VariableList::get_variable_id(const NomadString& name) const {
    return m_variable_map.get_variable_id(name);
}

const NomadString& VariableList::get_variable_name(NomadId variable_id) const {
    return m_variable_map.get_variable_name(variable_id);
}

const Type* VariableList::get_variable_type(NomadId variable_id) const {
    return m_variable_map.get_variable_type(variable_id);
}

NomadIndex VariableList::get_variable_count() const {
    return m_variable_map.get_variable_count();
}

void VariableList::set_variable_value(NomadId variable_id, const ScriptValue& value) {
#ifdef NOMAD_DEBUG
    if (variable_id >= m_variable_values.size()) {
        log::error("[VariableList::set_variable_value] Variable ID out of range");
    }
#endif

    m_variable_values[variable_id] = value;
}

void VariableList::get_variable_value(NomadId variable_id, ScriptValue& value) const {
#ifdef NOMAD_DEBUG
    if (variable_id >= m_variable_values.size()) {
        log::error("[VariableList::get_script_variable_value] Variable ID out of range");

        return;
    }
#endif

    value = m_variable_values[variable_id];
}

} // nomad