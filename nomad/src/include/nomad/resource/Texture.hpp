//
// Created by Jean-François Bilodeau on 2023-06-18.
//

#ifndef NOMAD_TEXTURE_HPP
#define NOMAD_TEXTURE_HPP

#include "nomad/resource/Resource.hpp"

#include <vector>

// Static forward declaration
struct SDL_Texture;

namespace nomad {

// Nomad forward declaration
class Game;
class ResourceManager;

///////////////////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////////////////
class Texture : public Resource {
public:
    Texture(const NomadString& name, const NomadString& file_name, Game* game);
    Texture(const NomadString& name, Game* game, int width, int height);
    Texture(const NomadString& name, SDL_Texture* texture);
    Texture(const Texture& other) = delete;
    ~Texture() override;

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;

    [[nodiscard]] SDL_Texture* get_sdl_texture() const;

private:
    SDL_Texture* m_texture = nullptr;
    int width = 0;
    int height = 0;
};

///////////////////////////////////////////////////////////////////////////////
// TextureManager
///////////////////////////////////////////////////////////////////////////////
class TextureManager {
public:
    explicit TextureManager(ResourceManager* resources);

    [[nodiscard]] NomadId register_texture(const NomadString& texture_name);
    [[nodiscard]] const Texture* get_texture(NomadId texture_id) const;
    [[nodiscard]] const Texture* get_texture_by_name(const NomadString& texture_name) const;

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Texture>> m_textures;
};

} // nomad

#endif //NOMAD_TEXTURE_HPP
