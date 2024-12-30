//
// Created by jfbil on 2023-07-18.
//

#include "nomad/game/EntityVariableContext.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Game.hpp"

namespace nomad {
EntityVariableContext::EntityVariableContext(Game* game):
    m_game(game)
{
}

void EntityVariableContext::set_value(NomadId variable_id, const ScriptValue& value) {
    const auto entity = m_game->get_current_context()->get_this_entity();

    if (entity) {
        entity->set_variable_value(variable_id, value);
    } else {
        log::error("EntityVariableContext::set_value: No entity in current context");
    }
}

void EntityVariableContext::get_value(NomadId variable_id, ScriptValue& value) {
    const auto entity = m_game->get_current_context()->get_this_entity();

    if (entity) {
        entity->get_variable_value(variable_id, value);
    } else {
       log::error("EntityVariableContext::set_value: No entity in current context");
    }
}

} // nomad