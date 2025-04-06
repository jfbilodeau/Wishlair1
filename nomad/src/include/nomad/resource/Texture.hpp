//
// Created by Jean-François Bilodeau on 2023-06-18.
//

#pragma once

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
    Texture(const NomadString& name, const NomadString& fileName, Game* game);
    Texture(const NomadString& name, Game* game, int width, int height);
    Texture(const NomadString& name, SDL_Texture* texture);
    Texture(const Texture& other) = delete;
    ~Texture() override;

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    [[nodiscard]] SDL_Texture* getSdlTexture() const;

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

    [[nodiscard]] NomadId registerTexture(const NomadString& textureName);
    [[nodiscard]] const Texture* getTexture(NomadId textureId) const;
    [[nodiscard]] const Texture* getTextureByName(const NomadString& textureName) const;

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Texture>> m_textures;
};

} // nomad

