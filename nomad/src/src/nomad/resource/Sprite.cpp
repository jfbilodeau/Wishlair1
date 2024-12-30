//
// Created by Jean-François Bilodeau on 2023-06-18.
//

#include "nomad/resource/Sprite.hpp"

#include "nomad/resource/Texture.hpp"
#include "nomad/resource/ResourceManager.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Sprite
Sprite::Sprite(const NomadString& sprite_name, const Texture* texture, const Rectangle& source, const Rectangle& frame):
    m_name(sprite_name),
    m_texture(texture),
    m_source(source),
    m_frame(frame)
{
}

const NomadString& Sprite::get_name() const {
    return m_name;
}

const Rectangle& Sprite::get_source() const {
    return m_source;
}

const Rectangle& Sprite::get_frame() const {
    return m_frame;
}

const Texture* Sprite::get_texture() const {
    return m_texture;
}

NomadInteger Sprite::get_width() const {
    return m_frame.get_width();
}

NomadInteger Sprite::get_height() const {
    return m_frame.get_height();
}

///////////////////////////////////////////////////////////////////////////////
// SpriteManager
SpriteManager::SpriteManager(ResourceManager* resources):
    m_resources(resources)
{}


NomadId SpriteManager::register_sprite(
    const NomadString& sprite_name, const Texture* texture, const Rectangle& source, const Rectangle& frame
) {
    auto id = to_nomad_id(m_sprites.size());

    m_sprites.emplace_back(
        std::make_unique<Sprite>(
            sprite_name,
            texture,
            source,
            frame
        )
    );

    return id;
}

const Sprite* SpriteManager::get_sprite(NomadId sprite_id) const {
    if (sprite_id >= m_sprites.size()) {
        return nullptr;
    }

    return m_sprites[sprite_id].get();
}

const Sprite* SpriteManager::get_sprite_by_name(const NomadString& name) const {
    for (const auto& sprite : m_sprites) {
        if (sprite->get_name() == name) {
            return sprite.get();
        }
    }

    return nullptr;
}

void SpriteManager::get_sprites_by_prefix(const NomadString& prefix, std::vector<Sprite*>& sprites) const {
    for (const auto& sprite : m_sprites) {
        if (sprite->get_name().compare(0, prefix.size(), prefix) == 0) {
            sprites.push_back(sprite.get());
        }
    }
}

} // nomad