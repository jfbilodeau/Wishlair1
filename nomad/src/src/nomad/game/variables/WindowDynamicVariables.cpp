//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::initWindowDynamicVariables() {
    log::debug("Initializing window dynamic variables");

    m_runtime->registerDynamicVariable(
        "window.title",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            const auto title = value.getStringValue();

            SDL_SetWindowTitle(m_window, title);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            const auto title = SDL_GetWindowTitle(m_window);

            interpreter->setStringResult(title);
        },
        getRuntime()->getStringType(),
        NomadDoc("Set the title of the game window.")
    );

    m_runtime->registerDynamicVariable(
        "window.fps",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_fps = int(value.getIntegerValue());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(m_fps);
        },
        getRuntime()->getIntegerType(),
        NomadDoc("The frames per second (FPS) of the game.")
    );

    m_runtime->registerDynamicVariable(
        "window.width",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_windowSize.setX(int(value.getIntegerValue()));

            SDL_SetWindowSize(m_window, m_windowSize.getX(), m_windowSize.getY());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(m_windowSize.getX());
        },
        getRuntime()->getIntegerType(),
        NomadDoc("The width of the game window.")
    );

    m_runtime->registerDynamicVariable(
        "window.height",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_windowSize.setY(int(value.getIntegerValue()));

            SDL_SetWindowSize(m_window, m_windowSize.getX(), m_windowSize.getY());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(m_windowSize.getY());
        },
        getRuntime()->getIntegerType(),
        NomadDoc("The height of the game window.")
    );

    m_runtime->registerDynamicVariable(
        "window.resolution.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_resolution.setX(int(value.getIntegerValue()));
            SDL_RenderSetLogicalSize(m_renderer, m_resolution.getX(), m_resolution.getY());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(m_resolution.getX());
        },
        getRuntime()->getIntegerType(),
        NomadDoc("The horizontal resolution of the game.")
    );

    m_runtime->registerDynamicVariable(
        "window.resolution.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_resolution.setY(int(value.getIntegerValue()));
            SDL_RenderSetLogicalSize(m_renderer, m_resolution.getX(), m_resolution.getY());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(m_resolution.getY());
        },
        getRuntime()->getIntegerType(),
        NomadDoc("The vertical resolution of the game.")
    );
}

} // nomad