//
// Created by jfbil on 2023-07-18.
//

#pragma once

#include "nomad/script/VariableContext.hpp"

namespace nomad {

const NomadString THIS_ENTITY_VARIABLE_CONTEXT = "this";
const NomadString OTHER_ENTITY_VARIABLE_CONTEXT = "other";
const NomadString THIS_ENTITY_VARIABLE_PREFIX = THIS_ENTITY_VARIABLE_CONTEXT + ".";
const NomadString OTHER_ENTITY_VARIABLE_PREFIX = OTHER_ENTITY_VARIABLE_CONTEXT + ".";

// Forward declarations
class Game;

class ThisEntityVariableContext : public VariableContext {
public:
    explicit ThisEntityVariableContext(Game* game);
    ThisEntityVariableContext(const ThisEntityVariableContext&) = delete;
    ~ThisEntityVariableContext() override = default;

    NomadId register_variable(const NomadString &name, const Type* type) override;
    const NomadString & get_variable_name(NomadId variable_id) const override;
    NomadId get_variable_id(const NomadString &name) const override;

    void set_variable_type(NomadId variable_id, const Type* type) override;
    const Type* get_variable_type(NomadId variable_id) const override;

    void set_value(NomadId variable_id, const ScriptValue& value) override;
    void get_value(NomadId variable_id, ScriptValue& value) override;

    [[nodiscard]]
    Game* get_game() const;

    [[nodiscard]]
    const VariableMap* get_this_variable_map() const;
    [[nodiscard]]
    const VariableMap* get_other_variable_map() const;

private:
    Game* m_game;
    VariableMap m_this_entity_variable_map;
    VariableMap m_other_entity_variable_map;
};

class OtherEntityVariableContext : public VariableContext {
public:
    explicit OtherEntityVariableContext(ThisEntityVariableContext* this_entity_variable_context);
    OtherEntityVariableContext(const OtherEntityVariableContext&) = delete;
    ~OtherEntityVariableContext() override = default;

    NomadId register_variable(const NomadString &name, const Type *type) override;
    const NomadString & get_variable_name(NomadId variable_id) const override;
    NomadId get_variable_id(const NomadString &name) const override;

    void set_variable_type(NomadId variable_id, const Type *type) override;
    const Type * get_variable_type(NomadId variable_id) const override;

    void set_value(NomadId variable_id, const ScriptValue& value) override;
    void get_value(NomadId variable_id, ScriptValue& value) override;

private:
    ThisEntityVariableContext* m_this_entity_variable_context;
};

} // nomad

