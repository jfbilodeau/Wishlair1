//
// Created by Jean-François Bilodeau on 2023-06-18.
//

#include "nomad/resource/Texture.hpp"

#include "nomad/game/Canvas.hpp"
#include "nomad/game/Game.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_render.h"

#include <cassert>

namespace nomad {

Texture::Texture(const NomadString& name, const NomadString& fileName, Game* game):
    Resource(name)
{
    auto surface = IMG_Load(fileName.c_str());

    SDL_Renderer* renderer = game->getCanvas()->getSdlRenderer();

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_QueryTexture(m_texture, nullptr, nullptr, &width, &height);

    SDL_FreeSurface(surface);

    if (m_texture == nullptr) {
        throw ResourceException("Failed to load image: " + fileName + "\n" + IMG_GetError());
    }
}

Texture::Texture(const NomadString &name, Game* game, int width, int height):
    Resource(name)
{
    auto renderer = game->getCanvas()->getSdlRenderer();

    m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    if (m_texture == nullptr) {
        throw ResourceException("Failed to create texture. Reason: " + NomadString(IMG_GetError()));
    }
}

Texture::Texture(const NomadString &name, SDL_Texture *texture):
    Resource(name),
    m_texture(texture)
{
    SDL_QueryTexture(m_texture, nullptr, nullptr, &width, &height);
}

Texture::~Texture() {
    if (m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);

        m_texture = nullptr;
    }
}

int Texture::getWidth() const { return width; }

int Texture::getHeight() const { return height; }

SDL_Texture* Texture::getSdlTexture() const {
    assert(m_texture != nullptr);

    return m_texture;
}

///////////////////////////////////////////////////////////////////////////////
// TextureManager
TextureManager::TextureManager(ResourceManager* resources):
    m_resources(resources)
{
}

NomadId TextureManager::registerTexture(const NomadString& textureName) {
    log::debug("Loading texture: " + textureName);

    const NomadString file_name = m_resources->makeResourcePath(textureName);

    auto texture_id = to_nomad_id(m_textures.size());

    m_textures.emplace_back(
        std::make_unique<Texture>(
            textureName,
            file_name,
            m_resources->getGame()
        )
    );

    return texture_id;
}

const Texture* TextureManager::getTexture(NomadId textureId) const {
    if (textureId >= m_textures.size()) {
        return nullptr;
    }

    return m_textures[textureId].get();
}

const Texture* TextureManager::getTextureByName(const NomadString& textureName) const {
    for (const auto& texture : m_textures) {
        if (texture->getName() == textureName) {
            return texture.get();
        }
    }

    return nullptr;
}


} // nomad