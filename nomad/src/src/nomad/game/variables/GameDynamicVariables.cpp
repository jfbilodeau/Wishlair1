//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::init_game_dynamic_variables() {
    log::debug("Initializing game dynamic variables");

    m_runtime->register_dynamic_variable(
        "game.organization",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            NomadString organization = value.get_string_value();

            m_organization = organization;
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_string_value(m_organization);
        },
        m_runtime->get_string_type(),
        NomadDoc("The organization who made the game.")
    );

    m_runtime->register_dynamic_variable(
        "game.name",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            NomadString name = value.get_string_value();

            m_name = name;
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_string_value(m_name);
        },
        m_runtime->get_string_type(),
        NomadDoc("The name of the game.")
    );
}

} // nomad