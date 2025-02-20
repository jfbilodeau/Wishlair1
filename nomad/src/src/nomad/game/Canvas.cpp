//
// Created by Jean-François Bilodeau on 2023-06-19.
//

#include "nomad/game/Canvas.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/resource/Sprite.hpp"
#include "nomad/resource/Texture.hpp"

namespace nomad {

Canvas::Canvas(Game* game, SDL_Renderer* renderer):
    m_game(game),
    m_renderer(renderer)
{
}

SDL_Renderer* Canvas::get_sdl_renderer() const {
    return m_renderer;
}

Game* Canvas::get_game() const {
    return m_game;
}

void Canvas::clear(const Color& color) {
    int result = SDL_SetRenderDrawColor(
        m_renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    if (result != 0) {
        throw GameException("Failed to set render draw color: " + NomadString(SDL_GetError()));
    }

    result = SDL_RenderClear(m_renderer);

    if (result != 0) {
        throw GameException("Failed to clear renderer: " + NomadString(SDL_GetError()));
    }
}

void Canvas::set_offset(const PointF &offset) {
    m_offset = offset;
}

void Canvas::set_offset(Coord x, Coord y) {
    set_offset({x, y});
}

const PointF& Canvas::get_offset() const {
    return m_offset;
}

void Canvas::render_sprite(const Sprite* sprite, Coord x, Coord y) const {
    if (sprite->get_texture() == nullptr) {
        log::warning("Trying to render a sprite with a null texture");

        return;
    }

    auto& frame = sprite->get_frame();

    const auto& source = frame;

    int left = static_cast<int>(x) + static_cast<int>(sprite->get_source().get_left());
    int top = static_cast<int>(y) + static_cast<int>(sprite->get_source().get_top());
    const auto destination = Rectangle(
        left,
        top,
        frame.get_width(),
        frame.get_height()
    );

    render_texture(sprite->get_texture(), source, destination);
}

void Canvas::render_texture(const Texture* texture, const Rectangle& source, const Rectangle& destination) const {
    auto sdl_texture = texture->get_sdl_texture();

    auto src_rect = source.to_sdl_rect();

    auto dst_rect = destination.to_sdl_rect();

    dst_rect.x += static_cast<int>(m_offset.x());
    dst_rect.y += static_cast<int>(m_offset.y());

    int result = SDL_RenderCopy(m_renderer, sdl_texture, &src_rect, &dst_rect);

    if (result != 0) {
        log::error("Failed to render texture: " + NomadString(SDL_GetError()));
    }
}

} // nomad