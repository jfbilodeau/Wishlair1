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

    NomadId registerVariable(const NomadString &name, const Type* type) override;
    const NomadString & getVariableName(NomadId variableId) const override;
    NomadId getVariableId(const NomadString &name) const override;

    void setVariableType(NomadId variableId, const Type* type) override;
    const Type* getVariableType(NomadId variableId) const override;

    void setValue(NomadId variableId, const ScriptValue& value) override;
    void getValue(NomadId variableId, ScriptValue& value) override;

    [[nodiscard]]
    Game* getGame() const;

    [[nodiscard]]
    const VariableMap* getThisVariableMap() const;
    [[nodiscard]]
    const VariableMap* getOtherVariableMap() const;

private:
    Game* m_game;
    VariableMap m_thisEntityVariableMap;
    VariableMap m_otherEntityVariableMap;
};

class OtherEntityVariableContext : public VariableContext {
public:
    explicit OtherEntityVariableContext(ThisEntityVariableContext* thisEntityVariableContext);
    OtherEntityVariableContext(const OtherEntityVariableContext&) = delete;
    ~OtherEntityVariableContext() override = default;

    NomadId registerVariable(const NomadString &name, const Type *type) override;
    const NomadString & getVariableName(NomadId variableId) const override;
    NomadId getVariableId(const NomadString &name) const override;

    void setVariableType(NomadId variableId, const Type *type) override;
    const Type * getVariableType(NomadId variableId) const override;

    void setValue(NomadId variableId, const ScriptValue& value) override;
    void getValue(NomadId variableId, ScriptValue& value) override;

private:
    ThisEntityVariableContext* m_thisEntityVariableContext;
};

} // nomad

