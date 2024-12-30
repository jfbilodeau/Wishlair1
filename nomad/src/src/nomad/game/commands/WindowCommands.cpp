//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::init_window_commands() {
    log::debug("Initializing windows commands");

    m_runtime->register_command(
        "window.setResolution",
        [this](Interpreter* interpreter) {
            auto resolutionX = int(interpreter->get_integer_parameter(0));
            auto resolutionY = int(interpreter->get_integer_parameter(1));

            m_resolution.set(resolutionX, resolutionY);

            SDL_RenderSetLogicalSize(m_renderer, m_resolution.x(), m_resolution.y());
        }, {
            def_parameter("resolutionWidth", m_runtime->get_integer_type(), NomadParamDoc("Resolution width.")),
            def_parameter("resolutionHeight", m_runtime->get_integer_type(), NomadParamDoc("Resolution height."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Sets the resolution of the game window.")
    );

    m_runtime->register_command(
        "window.setSize",
        [this](Interpreter* interpreter) {
            auto width = int(interpreter->get_integer_parameter(0));
            auto height = int(interpreter->get_integer_parameter(1));

            m_window_size.set(width, height);

            SDL_SetWindowSize(m_window, m_window_size.x(), m_window_size.y());
        }, {
            def_parameter("windowWidth", m_runtime->get_integer_type(), NomadParamDoc("Window width.")),
            def_parameter("windowHeight", m_runtime->get_integer_type(), NomadParamDoc("Window height."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Sets the size of the game window.")
    );

    m_runtime->register_command(
        "window.setFps",
        [this](Interpreter* interpreter) {
            auto fps = int(interpreter->get_integer_parameter(0));

            m_fps = fps;
        }, {
            def_parameter("framesPerSecond", m_runtime->get_integer_type(), NomadParamDoc("Frames per second."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Sets the frames per seconds (FPS) of the game.")
    );

    m_runtime->register_command(
        "window.setTitle",
        [this](Interpreter* interpreter) {
            auto title = interpreter->get_string_parameter(0);

            SDL_SetWindowTitle(m_window, title);
        }, {
            def_parameter("windowTitle", m_runtime->get_string_type(), NomadParamDoc("Title of the game window.")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the title of the game window.")
    );
}

} // nomad