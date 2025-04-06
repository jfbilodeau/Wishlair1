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

void Game::initWindowCommands() {
    log::debug("Initializing windows commands");

    m_runtime->registerCommand(
        "window.setResolution",
        [this](Interpreter* interpreter) {
            auto resolutionX = int(interpreter->getIntegerParameter(0));
            auto resolutionY = int(interpreter->getIntegerParameter(1));

            m_resolution.set(resolutionX, resolutionY);

            SDL_RenderSetLogicalSize(m_renderer, m_resolution.getX(), m_resolution.getY());
        }, {
            defParameter("resolutionWidth", m_runtime->getIntegerType(), NomadParamDoc("Resolution width.")),
            defParameter("resolutionHeight", m_runtime->getIntegerType(), NomadParamDoc("Resolution height."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Sets the resolution of the game window.")
    );

    m_runtime->registerCommand(
        "window.setSize",
        [this](Interpreter* interpreter) {
            auto width = int(interpreter->getIntegerParameter(0));
            auto height = int(interpreter->getIntegerParameter(1));

            m_windowSize.set(width, height);

            SDL_SetWindowSize(m_window, m_windowSize.getX(), m_windowSize.getY());
        }, {
            defParameter("windowWidth", m_runtime->getIntegerType(), NomadParamDoc("Window width.")),
            defParameter("windowHeight", m_runtime->getIntegerType(), NomadParamDoc("Window height."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Sets the size of the game window.")
    );

    m_runtime->registerCommand(
        "window.setFps",
        [this](Interpreter* interpreter) {
            auto fps = int(interpreter->getIntegerParameter(0));

            m_fps = fps;
        }, {
            defParameter("framesPerSecond", m_runtime->getIntegerType(), NomadParamDoc("Frames per second."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Sets the frames per seconds (FPS) of the game.")
    );

    m_runtime->registerCommand(
        "window.setTitle",
        [this](Interpreter* interpreter) {
            auto title = interpreter->getStringParameter(0);

            SDL_SetWindowTitle(m_window, title);
        }, {
            defParameter("windowTitle", m_runtime->getStringType(), NomadParamDoc("Title of the game window.")),
        },
        m_runtime->getVoidType(),
        NomadDoc("Set the title of the game window.")
    );
}

} // nomad