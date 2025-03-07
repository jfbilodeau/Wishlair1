//
// Created by jfbil on 2023-07-18.
//

#include "nomad/game/EntityVariableContext.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Game.hpp"

namespace nomad {
ThisEntityVariableContext::ThisEntityVariableContext(Game* game):
    m_game(game)
{
}

NomadId ThisEntityVariableContext::register_variable(const NomadString &name, const Type *type) {
    NomadString this_name, other_name;

    if (name.starts_with(THIS_ENTITY_VARIABLE_PREFIX)) {
        this_name = name;
        other_name = OTHER_ENTITY_VARIABLE_PREFIX + name.substr(THIS_ENTITY_VARIABLE_PREFIX.size());
    } else if (name.starts_with(OTHER_ENTITY_VARIABLE_PREFIX)) {
        this_name = THIS_ENTITY_VARIABLE_PREFIX + name.substr(OTHER_ENTITY_VARIABLE_PREFIX.size());
        other_name = name;
    } else {
        log::error("[EntityVariableContext::register_variable] Unexpected variable name '" + name + "'");
        return NOMAD_INVALID_ID;
    }

    auto this_id = m_this_entity_variable_map.register_variable(this_name, type);
    auto other_id = m_other_entity_variable_map.register_variable(other_name, type);

    // Sanity check...
    if (this_id != other_id) {
        log::error("[EntityVariableContext::register_variable] Variable IDs do not match");
    }

    return this_id;
}

const NomadString & ThisEntityVariableContext::get_variable_name(NomadId variable_id) const {
    return m_this_entity_variable_map.get_variable_name(variable_id);
}

NomadId ThisEntityVariableContext::get_variable_id(const NomadString &name) const {
    if (name.starts_with(THIS_ENTITY_VARIABLE_PREFIX)) {
        return m_this_entity_variable_map.get_variable_id(name);
    } else if (name.starts_with(OTHER_ENTITY_VARIABLE_PREFIX)) {
        return m_other_entity_variable_map.get_variable_id(name);
    } else {
        log::error("[EntityVariableContext::get_variable_id] Unexpected variable name '" + name + "'");
        return NOMAD_INVALID_ID;
    }
}

void ThisEntityVariableContext::set_variable_type(NomadId variable_id, const Type *type) {
    m_this_entity_variable_map.set_variable_type(variable_id, type);
    m_other_entity_variable_map.set_variable_type(variable_id, type);
}

const Type* ThisEntityVariableContext::get_variable_type(NomadId variable_id) const {
    return m_this_entity_variable_map.get_variable_type(variable_id);
}

void ThisEntityVariableContext::set_value(NomadId variable_id, const ScriptValue& value) {
    const auto entity = m_game->get_current_context()->get_this_entity();

    if (entity) {
        entity->set_variable_value(variable_id, value);
    } else {
        log::error("EntityVariableContext::set_value: No entity in current context");
    }
}

void ThisEntityVariableContext::get_value(NomadId variable_id, ScriptValue& value) {
    const auto entity = m_game->get_current_context()->get_this_entity();

    if (entity) {
        entity->get_variable_value(variable_id, value);
    } else {
       log::error("EntityVariableContext::set_value: No entity in current context");
    }
}

Game * ThisEntityVariableContext::get_game() const {
    return m_game;
}

const VariableMap* ThisEntityVariableContext::get_this_variable_map() const {
    return &m_this_entity_variable_map;
}

const VariableMap * ThisEntityVariableContext::get_other_variable_map() const {
    return &m_other_entity_variable_map;
}

OtherEntityVariableContext::OtherEntityVariableContext(ThisEntityVariableContext *this_entity_variable_context):
    m_this_entity_variable_context(this_entity_variable_context)
{
}

NomadId OtherEntityVariableContext::register_variable(const NomadString &name, const Type *type) {
    return m_this_entity_variable_context->register_variable(name, type);
}

const NomadString & OtherEntityVariableContext::get_variable_name(NomadId variable_id) const {
    return m_this_entity_variable_context->get_other_variable_map()->get_variable_name(variable_id);
}

NomadId OtherEntityVariableContext::get_variable_id(const NomadString &name) const {
    return m_this_entity_variable_context->get_other_variable_map()->get_variable_id(name);
}

void OtherEntityVariableContext::set_variable_type(NomadId variable_id, const Type *type) {
    return m_this_entity_variable_context->set_variable_type(variable_id, type);
}

const Type* OtherEntityVariableContext::get_variable_type(NomadId variable_id) const {
    return m_this_entity_variable_context->get_variable_type(variable_id);
}

void OtherEntityVariableContext::set_value(NomadId variable_id, const ScriptValue &value) {
    m_this_entity_variable_context->get_game()->get_current_context()->for_each_other_entities([&](Entity* entity) {
        entity->set_variable_value(variable_id, value);
    });
}

void OtherEntityVariableContext::get_value(NomadId variable_id, ScriptValue &value) {
    auto entity = m_this_entity_variable_context->get_game()->get_current_context()->get_first_other_entity();

    if (entity) {
        entity->get_variable_value(variable_id, value);
    }
}

} // nomad