//
// Created by jfbil on 2023-06-13.
//

#ifndef NOMAD_VARIABLECONTEXT_HPP
#define NOMAD_VARIABLECONTEXT_HPP

#include "nomad/nomad.hpp"

#include "nomad/script/Variable.hpp"

namespace nomad {

class Type;

class VariableContext {
public:
    virtual ~VariableContext() = default;

    virtual NomadId register_variable(const NomadString& name, const Type* type) = 0;
    virtual const NomadString& get_variable_name(NomadId variable_id) const = 0;

    virtual NomadId get_variable_id(const NomadString& name) const = 0;
    virtual void set_variable_type(NomadId variable_id, const Type* type) = 0;
    virtual const Type* get_variable_type(NomadId variable_id) const = 0;

    virtual void set_value(NomadId variable_id, const ScriptValue& value) = 0;
    virtual void get_value(NomadId variable_id, ScriptValue& value) = 0;
};

class SimpleVariableContext : public VariableContext {
public:
    SimpleVariableContext();
    SimpleVariableContext(const SimpleVariableContext&) = delete;
    ~SimpleVariableContext() override = default;

    NomadId register_variable(const NomadString& name, const Type* type) override;
    [[nodiscard]]
    const NomadString& get_variable_name(NomadId variable_id) const override;

    [[nodiscard]]
    NomadId get_variable_id(const NomadString& name) const override;
    void set_variable_type(NomadId variable_id, const Type* type) override;
    [[nodiscard]]
    const Type* get_variable_type(NomadId variable_id) const override;

    void set_value(NomadId variable_id, const ScriptValue& value) override;
    void get_value(NomadId variable_id, ScriptValue& value) override;

    [[nodiscard]]
    const VariableMap* get_variable_map() const;

private:
    VariableMap m_variables;
    std::vector<ScriptValue> m_values;
};

} // namespace nomad

#endif // NOMAD_VARIABLECONTEXT_HPP
