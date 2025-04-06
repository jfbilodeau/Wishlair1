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

void Game::initGameCommands() {
    log::debug("Initializing game commands");

    m_runtime->registerCommand(
        "game.createScene",
        [this](Interpreter* interpreter) {
            NomadString script_name = interpreter->getStringParameter(0);

            const auto scene = createScene();

            const auto script_id = getScriptId(script_name);

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script for scene '" + script_name + "' not found");
                return;
            }

            executeScriptInNewContext(script_id, scene, nullptr);
        }, {
            defParameter(
                "scriptName", m_runtime->getStringType(),
                NomadParamDoc("Name of the script to execute to initialize the scene.")
            )
        },
        m_runtime->getVoidType(),
        NomadDoc("Creates a new scene.")
    );

    m_runtime->registerCommand(
        "game.loadSpriteAtlas",
        [this](Interpreter* interpreter) {
            auto atlas_name = interpreter->getStringParameter(0);

            m_resourceManager->loadSpriteAtlas(atlas_name);
        }, {
            defParameter(
                "atlasName", m_runtime->getStringType(), NomadParamDoc("Name of the sprite atlas to load.")
            ),
        },
        m_runtime->getVoidType(),
        NomadDoc("Loads a sprite atlas from a file.")
    );

    m_runtime->registerCommand(
        "game.loadFont",
        [this](Interpreter* interpreter) {
            auto font_name = interpreter->getStringParameter(0);
            auto font_size = interpreter->getIntegerParameter(1);

            auto font_id = m_resourceManager->getFonts()->registerFont(font_name, static_cast<int>(font_size));

            interpreter->setIdResult(font_id);
        }, {
            defParameter("fontName", m_runtime->getStringType(), NomadParamDoc("Name of the font to load.")),
            defParameter(
                "fontSize", m_runtime->getIntegerType(), NomadParamDoc("Size of the font in point to load.")
            )
        },
        m_runtime->getIntegerType(),
        NomadDoc("Loads a font from a file.")
    );

    m_runtime->registerCommand(
        "game.loadImage",
        [this](Interpreter* interpreter) {
            NomadString texture_name = interpreter->getStringParameter(0);

            auto texture_file_name = texture_name + ".png";

            auto font_id = m_resourceManager->getTextures()->registerTexture(texture_file_name);

            interpreter->setIdResult(font_id);
        }, {
            defParameter("imageName", m_runtime->getStringType(), NomadParamDoc("Name of the image to load.")),
        },
        m_runtime->getIntegerType(),
        NomadDoc("Loads a font from a file.")
    );
}

} // namespace nomad