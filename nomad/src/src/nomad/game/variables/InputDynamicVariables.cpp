//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::initInputDynamicVariables() {
    log::debug("Initializing input dynamic variables");

    m_runtime->registerDynamicVariable(
        "input.mouse.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_mousePosition.setX(value.getFloatValue());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setFloatValue(m_mousePosition.getX());
        },
        m_runtime->getFloatType(),
        NomadDoc("The x position of the mouse.")
    );

    m_runtime->registerDynamicVariable(
        "input.mouse.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_mousePosition.setY(value.getFloatValue());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setFloatValue(m_mousePosition.getY());
        },
        m_runtime->getFloatType(),
        NomadDoc("The y position of the mouse.")
    );

    m_runtime->registerDynamicVariable(
        "input.mouse.deltaX",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setFloatValue(m_mouseLastPosition.getX() - m_mousePosition.getX());
        },
        m_runtime->getFloatType(),
        NomadDoc("The change in x position of the mouse since last frame.")
    );

    m_runtime->registerDynamicVariable(
        "input.mouse.deltaY",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setFloatValue(m_mouseLastPosition.getY() - m_mousePosition.getY());
        },
        m_runtime->getFloatType(),
        NomadDoc("The change in y position of the mouse since last frame.")
    );
}

} // nomad