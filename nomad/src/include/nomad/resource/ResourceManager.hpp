//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#ifndef NOMAD_RESOURCEMANAGER_HPP
#define NOMAD_RESOURCEMANAGER_HPP

#include "nomad/resource/Resource.hpp"

#include "nomad/resource/Animation.hpp"
#include "nomad/resource/Font.hpp"
#include "nomad/resource/Sprite.hpp"
#include "nomad/resource/Text.hpp"
#include "nomad/resource/Texture.hpp"

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

class Game;

class ResourceException : public NomadException {
public:
    explicit ResourceException(const NomadString& message) : NomadException(message) {}
};

class ResourceManager {
public:
    explicit ResourceManager(Game* game, const NomadString& base_path);
    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) noexcept = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ~ResourceManager();

    [[nodiscard]] Game* get_game() const;

    [[nodiscard]] const NomadString& get_base_path() const;
    [[nodiscard]] NomadString make_resource_path(const NomadString& resource_name) const;

    // Individual resource managers
    AnimationManager* get_animations();
    FontManager* get_fonts();
    SpriteManager* get_sprites();
    TextManager* get_text();
    TextureManager* get_textures();

    // TODO: These specialized method should be move elsewhere...
    void load_sprite_atlas(const NomadString& resource_name);

private:
    NomadString make_resource_name(const NomadString& base_path, const NomadString& file_name);

    Game* m_game;
    NomadString m_base_path;
    AnimationManager m_animations;
    FontManager m_fonts;
    SpriteManager m_sprites;
    TextManager m_text;
    TextureManager m_textures;
};

} // nomad

#endif //NOMAD_RESOURCEMANAGER_HPP
