//
// Created by Jean-François Bilodeau on 2023-06-18.
//

#ifndef NOMAD_SPRITE_HPP
#define NOMAD_SPRITE_HPP

#include "nomad/geometry/Point.hpp"
#include "nomad/geometry/Rectangle.hpp"

#include "nomad/resource/Resource.hpp"

namespace nomad {

class Texture;

///////////////////////////////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////////////////////////////
class Sprite {
public:
    Sprite(const NomadString& sprite_name, const Texture* texture, const Rectangle& source, const Rectangle& frame);

    [[nodiscard]] const NomadString& get_name() const;
    [[nodiscard]] const Rectangle& get_source() const;
    [[nodiscard]] const Rectangle& get_frame() const;
    [[nodiscard]] const Texture* get_texture() const;

    [[nodiscard]] NomadInteger get_width() const;
    [[nodiscard]] NomadInteger get_height() const;

private:
    NomadString m_name;
    const Texture* m_texture;
    Rectangle m_source;
    Rectangle m_frame;
};

///////////////////////////////////////////////////////////////////////////////
// SpriteManager
///////////////////////////////////////////////////////////////////////////////
class SpriteManager {
public:
    explicit SpriteManager(ResourceManager* resources);

    [[nodiscard]] NomadId register_sprite(
        const NomadString& sprite_name,
        const Texture* texture,
        const Rectangle& source,
        const Rectangle& frame
    );
    [[nodiscard]] const Sprite* get_sprite(NomadId sprite_id) const;
    [[nodiscard]] const Sprite* get_sprite_by_name(const NomadString& name) const;
    void get_sprites_by_prefix(const NomadString& prefix, std::vector<Sprite*>& sprites) const;

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Sprite>> m_sprites;
};

} // namespace nomad

#endif //NOMAD_SPRITE_HPP
