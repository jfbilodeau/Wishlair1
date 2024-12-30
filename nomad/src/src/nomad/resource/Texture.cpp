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

Texture::Texture(const NomadString& name, const NomadString& file_name, Game* game):
    Resource(name)
{
    auto surface = IMG_Load(file_name.c_str());

    SDL_Renderer* renderer = game->get_canvas()->get_sdl_renderer();

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_QueryTexture(m_texture, nullptr, nullptr, &width, &height);

    SDL_FreeSurface(surface);

    if (m_texture == nullptr) {
        throw ResourceException("Failed to load image: " + file_name + "\n" + IMG_GetError());
    }
}

Texture::~Texture() {
    if (m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);

        m_texture = nullptr;
    }
}

int Texture::get_width() const { return width; }

int Texture::get_height() const { return height; }

SDL_Texture* Texture::get_sdl_texture() const {
    assert(m_texture != nullptr);

    return m_texture;
}

///////////////////////////////////////////////////////////////////////////////
// TextureManager
TextureManager::TextureManager(ResourceManager* resources):
    m_resources(resources)
{
}

NomadId TextureManager::register_texture(const NomadString& texture_name) {
    log::debug("Loading texture: " + texture_name);

    const NomadString file_name = m_resources->make_resource_path(texture_name);

    auto texture_id = to_nomad_id(m_textures.size());

    m_textures.emplace_back(
        std::make_unique<Texture>(
            texture_name,
            file_name,
            m_resources->get_game()
        )
    );

    return texture_id;
}

const Texture* TextureManager::get_texture(NomadId texture_id) const {
    if (texture_id >= m_textures.size()) {
        return nullptr;
    }

    return m_textures[texture_id].get();
}

const Texture* TextureManager::get_texture_by_name(const NomadString& texture_name) const {
    for (const auto& texture : m_textures) {
        if (texture->get_name() == texture_name) {
            return texture.get();
        }
    }

    return nullptr;
}


} // nomad