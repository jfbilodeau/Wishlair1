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

void Game::init_game_commands() {
    log::debug("Initializing game commands");

    m_runtime->register_command(
        "game.createScene",
        [this](Interpreter* interpreter) {
            NomadString script_name = interpreter->get_string_parameter(0);

            const auto scene = create_scene();

            const auto script_id = get_script_id(script_name);

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script for scene '" + script_name + "' not found");
                return;
            }

            execute_script_in_new_context(script_id, scene, nullptr);
        }, {
            def_parameter(
                "scriptName", m_runtime->get_string_type(),
                NomadParamDoc("Name of the script to execute to initialize the scene.")
            )
        },
        m_runtime->get_void_type(),
        NomadDoc("Creates a new scene.")
    );

    m_runtime->register_command(
        "game.loadSpriteAtlas",
        [this](Interpreter* interpreter) {
            auto atlas_name = interpreter->get_string_parameter(0);

            m_resource_manager->load_sprite_atlas(atlas_name);
        }, {
            def_parameter(
                "atlasName", m_runtime->get_string_type(), NomadParamDoc("Name of the sprite atlas to load.")
            ),
        },
        m_runtime->get_void_type(),
        NomadDoc("Loads a sprite atlas from a file.")
    );

    m_runtime->register_command(
        "game.loadFont",
        [this](Interpreter* interpreter) {
            auto font_name = interpreter->get_string_parameter(0);
            auto font_size = interpreter->get_integer_parameter(1);

            auto font_id = m_resource_manager->get_fonts()->register_font(font_name, static_cast<int>(font_size));

            interpreter->set_id_result(font_id);
        }, {
            def_parameter("fontName", m_runtime->get_string_type(), NomadParamDoc("Name of the font to load.")),
            def_parameter(
                "fontSize", m_runtime->get_integer_type(), NomadParamDoc("Size of the font in point to load.")
            )
        },
        m_runtime->get_integer_type(),
        NomadDoc("Loads a font from a file.")
    );

    m_runtime->register_command(
        "game.loadImage",
        [this](Interpreter* interpreter) {
            NomadString texture_name = interpreter->get_string_parameter(0);

            auto texture_file_name = texture_name + ".png";

            auto font_id = m_resource_manager->get_textures()->register_texture(texture_file_name);

            interpreter->set_id_result(font_id);
        }, {
            def_parameter("imageName", m_runtime->get_string_type(), NomadParamDoc("Name of the image to load.")),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Loads a font from a file.")
    );
}

} // namespace nomad