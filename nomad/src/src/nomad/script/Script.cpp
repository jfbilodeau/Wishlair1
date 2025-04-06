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


void Script::add_parameter(const NomadString& parameter_name, const Type* type) {
    m_parameters.registerVariable(parameter_name, type);
}

NomadId Script::getParameterId(const NomadString& parameter_name) const {
    return m_parameters.getVariableId(parameter_name);
}

const NomadString& Script::getParameterName(NomadId parameter_id) const {
    return m_parameters.getVariableName(parameter_id);
}

const Type* Script::getParameterType(NomadId parameter_id) const {
    return m_parameters.getVariableType(parameter_id);
}

NomadIndex Script::getParameterCount() const {
    return m_parameters.getVariableCount();
}


NomadId Script::registerVariable(const NomadString& variable_name, const Type* type) {
    auto variable_id = m_variables.getVariableId(variable_name);

    if (variable_id != NOMAD_INVALID_ID) {
        auto current_type = m_variables.getVariableType(variable_id);

        if (current_type == nullptr) {
            m_variables.setVariableType(variable_id, type);
        } else if (current_type != type) {
            throw NomadException("[Script::register_variable] Internal error: Variable '" + variable_name + "' already exists with a different type");
        }

        return variable_id;
    }

    return m_variables.registerVariable(variable_name, type);
}

NomadId Script::getVariableId(const NomadString& variable_name) const {
    return m_variables.getVariableId(variable_name);
}

[[nodiscard]] const NomadString& Script::get_variable_name(NomadId variable_id) const {
    return m_variables.getVariableName(variable_id);
}

void Script::set_variable_type(NomadId variable_id, const Type* type) {
    m_variables.setVariableType(variable_id, type);
}

const Type* Script::getVariableType(NomadId variable_id) const {
    return m_variables.getVariableType(variable_id);
}

NomadIndex Script::getVariableCount() const {
    return m_variables.getVariableCount();
}

void Script::setReturnType(const Type* return_type) {
    m_return_type = return_type;
}

const Type* Script::getReturnType() const {
    return m_return_type;
}

//NomadId Script::register_format_string(const FormatString& format_string) {
//    const auto id = to_nomad_id(m_format_strings.size());
//
//    m_format_strings.emplace_back(format_string);
//
//    return id;
//}
//
//NomadId Script::get_format_string_id(const NomadString& format_string) const {
//    for (auto i = 0; i < m_format_strings.size(); ++i) {
//        if (m_format_strings[i].get_format_string() == format_string) {
//            return to_nomad_id(i);
//        }
//    }
//
//    return NOMAD_INVALID_ID;
//}
//
//const FormatString& Script::get_format_string(NomadId id) const {
//    return m_format_strings[id];
//}

void Script::setScriptStart(NomadIndex script_start_index) {
    m_script_start_index = script_start_index;
}

NomadIndex Script::getScriptStart() const {
    return m_script_start_index;
}

void Script::setScriptEnd(NomadIndex script_end_index) {
    m_script_end_index = script_end_index;
}

NomadIndex Script::get_script_end() const {
    return m_script_end_index;
}

NomadIndex Script::get_script_length() const {
    return m_script_end_index - m_script_start_index;
}

//
//NomadInteger Script::get_integer(NomadIndex index) const {
//    const auto value = *reinterpret_cast<const NomadInteger *>(&m_op_codes[index]);
//
//    return value;
//}
//
//NomadFloat Script::get_float(NomadIndex index) const {
//    const auto value = *reinterpret_cast<const NomadFloat*>(&m_op_codes[index]);
//
//    return value;
//}

} // nomad