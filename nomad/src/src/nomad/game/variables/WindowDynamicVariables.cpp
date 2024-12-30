//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::init_window_dynamic_variables() {
    log::debug("Initializing window dynamic variables");

    m_runtime->register_dynamic_variable(
        "window.title",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            const auto title = value.get_string_value();

            SDL_SetWindowTitle(m_window, title);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            const auto title = SDL_GetWindowTitle(m_window);

            interpreter->set_string_result(title);
        },
        get_runtime()->get_string_type(),
        NomadDoc("Set the title of the game window.")
    );

    m_runtime->register_dynamic_variable(
        "window.fps",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_fps = int(value.get_integer_value());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(m_fps);
        },
        get_runtime()->get_integer_type(),
        NomadDoc("The frames per second (FPS) of the game.")
    );

    m_runtime->register_dynamic_variable(
        "window.width",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_window_size.set_x(int(value.get_integer_value()));

            SDL_SetWindowSize(m_window, m_window_size.x(), m_window_size.y());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(m_window_size.x());
        },
        get_runtime()->get_integer_type(),
        NomadDoc("The width of the game window.")
    );

    m_runtime->register_dynamic_variable(
        "window.height",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_window_size.set_y(int(value.get_integer_value()));

            SDL_SetWindowSize(m_window, m_window_size.x(), m_window_size.y());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(m_window_size.y());
        },
        get_runtime()->get_integer_type(),
        NomadDoc("The height of the game window.")
    );

    m_runtime->register_dynamic_variable(
        "window.resolution.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_resolution.set_x(int(value.get_integer_value()));
            SDL_RenderSetLogicalSize(m_renderer, m_resolution.x(), m_resolution.y());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(m_resolution.x());
        },
        get_runtime()->get_integer_type(),
        NomadDoc("The horizontal resolution of the game.")
    );

    m_runtime->register_dynamic_variable(
        "window.resolution.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            m_resolution.set_y(int(value.get_integer_value()));
            SDL_RenderSetLogicalSize(m_renderer, m_resolution.x(), m_resolution.y());
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(m_resolution.y());
        },
        get_runtime()->get_integer_type(),
        NomadDoc("The vertical resolution of the game.")
    );
}

} // nomad