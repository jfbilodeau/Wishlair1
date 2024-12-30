//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::init_input_dynamic_variables() {
    log::debug("Initializing input dynamic variables");

    m_runtime->register_dynamic_variable(
        "input.mouse.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_mouse_position.set_x(value.get_float_value());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_float_value(m_mouse_position.x());
        },
        m_runtime->get_float_type(),
        NomadDoc("The x position of the mouse.")
    );

    m_runtime->register_dynamic_variable(
        "input.mouse.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_mouse_position.set_y(value.get_float_value());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_float_value(m_mouse_position.y());
        },
        m_runtime->get_float_type(),
        NomadDoc("The y position of the mouse.")
    );

    m_runtime->register_dynamic_variable(
        "input.mouse.deltaX",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_float_value(m_mouse_last_position.x() - m_mouse_position.x());
        },
        m_runtime->get_float_type(),
        NomadDoc("The change in x position of the mouse since last frame.")
    );

    m_runtime->register_dynamic_variable(
        "input.mouse.deltaY",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_float_value(m_mouse_last_position.y() - m_mouse_position.y());
        },
        m_runtime->get_float_type(),
        NomadDoc("The change in y position of the mouse since last frame.")
    );
}

} // nomad