//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/resource/Animation.hpp"
#include "nomad/resource/Font.hpp"
#include "nomad/resource/Sprite.hpp"
#include "nomad/resource/SpriteAtlas.hpp"
#include "nomad/resource/Texture.hpp"

#include "nomad/game/Game.hpp"

#include <boost/json.hpp>

#include <filesystem>
#include <fstream>

namespace nomad {

ResourceManager::ResourceManager(Game* game, const NomadString& base_path) :
    m_game(game),
    m_animations(this),
    m_fonts(this),
    m_sprites(this),
    m_textures(this)
{
    if (base_path.empty() == false) {
        // Make sure base_path does not end with a path separator.
        if (base_path.back() == '\\' || base_path.back() == '/') {
            m_base_path = base_path.substr(0, base_path.size() - 1);
        } else {
            m_base_path = base_path;
        }
    }
}

ResourceManager::~ResourceManager() {
}

Game* ResourceManager::get_game() const {
    return m_game;
}

const NomadString& ResourceManager::get_base_path() const {
    return m_base_path;
}

NomadString ResourceManager::make_resource_path(const NomadString& resource_name) const {
    auto file_name = m_base_path + "/" + resource_name;

    // Adjust path for Windows.
    if (std::filesystem::path::preferred_separator == '\\') {
        std::replace(file_name.begin(), file_name.end(), '/', '\\');
    }

    return file_name;
}

AnimationManager* ResourceManager::get_animations() {
    return &m_animations;
}

FontManager* ResourceManager::get_fonts() {
    return &m_fonts;
}

SpriteManager* ResourceManager::get_sprites() {
    return &m_sprites;
}

TextManager* ResourceManager::get_text() {
    return &m_text;
}

TextureManager* ResourceManager::get_textures() {
    return &m_textures;
}

void ResourceManager::load_sprite_atlas(const NomadString& resource_name) {
    try {
        log::debug("Loading sprite atlas: '" + resource_name + "'");

        NomadString path = make_resource_path(resource_name);

        std::ifstream file(path);

        if (!file.is_open()) {
            throw ResourceException("Cloud not open file: '" + resource_name + "'");
        }

        boost::json::stream_parser parser;

        char buffer[1024];

        while (!file.eof()) {
            file.read(buffer, sizeof(buffer));
            parser.write(buffer, file.gcount());
        }

        auto json = parser.release().as_object();

        // Texture name is the resource name but with the extension replaced by .png
        NomadString texture_name = resource_name.substr(0, resource_name.find_last_of('.')) + ".png";

        auto texture_id = get_textures()->register_texture(texture_name);
        auto texture = get_textures()->get_texture(texture_id);

        auto sprites = json.at("sprites").as_array();

        for (auto sprite: sprites) {
            NomadString name = sprite.at("name").as_string().c_str();

            Rectangle source(
                static_cast<int>(sprite.at("source").at("x").as_int64()),
                static_cast<int>(sprite.at("source").at("y").as_int64()),
                static_cast<int>(sprite.at("source").at("width").as_int64()),
                static_cast<int>(sprite.at("source").at("height").as_int64())
            );

            Rectangle frame(
                static_cast<int>(sprite.at("frame").at("x").as_int64()),
                static_cast<int>(sprite.at("frame").at("y").as_int64()),
                static_cast<int>(sprite.at("frame").at("width").as_int64()),
                static_cast<int>(sprite.at("frame").at("height").as_int64())
            );

            log::debug("Loading sprite: '" + name + "'");

            auto sprite_id = m_sprites.register_sprite(name, texture, source, frame);

            if (sprite_id == NOMAD_INVALID_ID) {
                throw ResourceException("Internal error: Failed to register sprite: '" + name + "'");
            }
        }

        log::debug("Loading texture: '" + texture_name + "'");
    }
    catch (const boost::exception& e) {
        throw ResourceException("Failed to load sprite atlas: '" + resource_name + "'");
    }
    catch (const std::exception& e) {
        throw ResourceException("Failed to load sprite atlas: '" + resource_name + "' - " + e.what());
    }
}

NomadString ResourceManager::make_resource_name(const NomadString& base_path, const NomadString& file_name) {
    return base_path + "/" + file_name;
}

} // nomad