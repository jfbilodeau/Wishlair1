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

void SceneVariableContext::setValue(NomadId variable_id, const ScriptValue& value) {
    auto scene = m_game->getCurrentContext()->getScene();

    if (scene) {
        scene->setVariableValue(variable_id, value);
    } else {
        log::error("SceneVariableContext::set_value: No scene in current context");
    }
}

void SceneVariableContext::getValue(NomadId variable_id, ScriptValue& value) {
    const auto scene = m_game->getCurrentContext()->getScene();

    if (scene) {
        scene->getVariableValue(variable_id, value);
    } else {
       log::error("SceneVariableContext::set_value: No scene in current context");
    }
}

} // nomad