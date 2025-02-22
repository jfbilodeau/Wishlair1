//
// Created by jfbil on 2023-07-18.
//

#include "nomad/game/SceneVariableContext.hpp"

#include "nomad/game/Scene.hpp"
#include "nomad/game/Game.hpp"

namespace nomad {
SceneVariableContext::SceneVariableContext(Game* game):
    m_game(game)
{
}

void SceneVariableContext::set_value(NomadId variable_id, const ScriptValue& value) {
    auto scene = m_game->get_current_context()->get_scene();

    if (scene) {
        scene->set_variable_value(variable_id, value);
    } else {
        log::error("SceneVariableContext::set_value: No scene in current context");
    }
}

void SceneVariableContext::get_value(NomadId variable_id, ScriptValue& value) {
    const auto scene = m_game->get_current_context()->get_scene();

    if (scene) {
        scene->get_variable_value(variable_id, value);
    } else {
       log::error("SceneVariableContext::set_value: No scene in current context");
    }
}

} // nomad