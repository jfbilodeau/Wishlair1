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

void Canvas::render_sprite(const Sprite* sprite, Coord x, Coord y) {
    if (sprite->get_texture() == nullptr) {
        log::warning("Trying to render a sprite with a null texture");

        return;
    }

    const auto& source = sprite->get_frame();

    const auto destination = Rectangle(
        int(x) + int(sprite->get_source().get_left()),
        int(y) + int(sprite->get_source().get_top()),
        sprite->get_frame().get_width(),
        sprite->get_frame().get_height()
    );

    render_texture(sprite->get_texture(), source, destination);
}

void Canvas::render_texture(const Texture* texture, const Rectangle& source, const Rectangle& destination) {
    auto sdl_texture = texture->get_sdl_texture();

    SDL_Rect src_rect = source.to_sdl_rect();

    SDL_Rect dst_rect = destination.to_sdl_rect();

    int result = SDL_RenderCopy(m_renderer, sdl_texture, &src_rect, &dst_rect);

    if (result != 0) {
        throw GameException("Failed to render texture: " + NomadString(SDL_GetError()));
    }
}

} // nomad