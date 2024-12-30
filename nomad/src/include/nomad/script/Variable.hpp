//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_VARIABLE_HPP
#define NOMAD_VARIABLE_HPP

#include "nomad/script/Type.hpp"

#include "nomad/Nomad.hpp"

#include <vector>

namespace nomad {

class VariableDefinition {
public:
    VariableDefinition(const NomadString& name, const Type* type);

    [[nodiscard]] const NomadString& get_name() const;
    void set_type(const Type* type);
    [[nodiscard]] const Type* get_type() const;

private:
    NomadString m_name;
    const Type* m_type;
};

class VariableMap {
public:
    VariableMap() = default;

    NomadId register_variable(const NomadString& name, const Type* type);
    [[nodiscard]] NomadId get_variable_id(const NomadString& name) const;
    [[nodiscard]] const NomadString& get_variable_name(NomadId variable_id) const;
    [[nodiscard]] void set_variable_type(NomadId variable_id, const Type* type);
    [[nodiscard]] const Type* get_variable_type(NomadId variable_id) const;
    [[nodiscard]] NomadIndex get_variable_count() const;
private:
    std::vector<VariableDefinition> m_variables;
};

class VariableList {
public:
    explicit VariableList(const VariableMap* variable_map);
    ~VariableList();

    [[nodiscard]] NomadId get_variable_id(const NomadString& name) const;
    [[nodiscard]] const NomadString& get_variable_name(NomadId variable_id) const;
    [[nodiscard]] const Type* get_variable_type(NomadId variable_id) const;
    [[nodiscard]] NomadIndex get_variable_count() const;

    void set_variable_value(NomadId variable_id, const ScriptValue& value);
    void get_variable_value(NomadId variable_id, ScriptValue& value) const;

private:
    const VariableMap m_variable_map;
    std::vector<ScriptValue> m_variable_values;
};

} // nomad

#endif //NOMAD_VARIABLE_HPP
