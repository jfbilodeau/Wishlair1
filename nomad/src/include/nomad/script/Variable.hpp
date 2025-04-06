//
// Created by jfbil on 2023-06-07.
//

#pragma once

#include "nomad/script/Type.hpp"

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

class VariableDefinition {
public:
    VariableDefinition(const NomadString& name, const Type* type);

    [[nodiscard]] const NomadString& getName() const;
    void setType(const Type* type);
    [[nodiscard]] const Type* getType() const;

private:
    NomadString m_name;
    const Type* m_type;
};

class VariableMap {
public:
    VariableMap() = default;

    NomadId registerVariable(const NomadString& name, const Type* type);
    [[nodiscard]] NomadId getVariableId(const NomadString& name) const;
    [[nodiscard]] const NomadString& getVariableName(NomadId variableId) const;
    void setVariableType(NomadId variableId, const Type* type);
    [[nodiscard]] const Type* getVariableType(NomadId variableId) const;
    [[nodiscard]] NomadIndex getVariableCount() const;
private:
    std::vector<VariableDefinition> m_variables;
};

class VariableList {
public:
    explicit VariableList(const VariableMap* variableMap);
    ~VariableList();

    [[nodiscard]] NomadId getVariableId(const NomadString& name) const;
    [[nodiscard]] const NomadString& getVariableName(NomadId variableId) const;
    [[nodiscard]] const Type* getVariableType(NomadId variableId) const;
    [[nodiscard]] NomadIndex getVariableCount() const;

    void setVariableValue(NomadId variableId, const ScriptValue& value);
    void getVariableValue(NomadId variableId, ScriptValue& value) const;

private:
    const VariableMap m_variableMap;
    std::vector<ScriptValue> m_variableValues;
};

} // nomad
