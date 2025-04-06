//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::initGameDynamicVariables() {
    log::debug("Initializing game dynamic variables");

    m_runtime->registerDynamicVariable(
        "game.organization",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            NomadString organization = value.getStringValue();

            m_organization = organization;
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setStringValue(m_organization);
        },
        m_runtime->getStringType(),
        NomadDoc("The organization who made the game.")
    );

    m_runtime->registerDynamicVariable(
        "game.name",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            NomadString name = value.getStringValue();

            m_name = name;
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setStringValue(m_name);
        },
        m_runtime->getStringType(),
        NomadDoc("The name of the game.")
    );
}

} // nomad