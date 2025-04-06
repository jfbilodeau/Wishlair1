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

SDL_Renderer* Canvas::getSdlRenderer() const {
    return m_renderer;
}

Game* Canvas::getGame() const {
    return m_game;
}

void Canvas::clear(const Color& color) {
    int result = SDL_SetRenderDrawColor(
        m_renderer,
        color.red,
        color.green,
        color.blue,
        color.alpha
    );

    if (result != 0) {
        throw GameException("Failed to set render draw color: " + NomadString(SDL_GetError()));
    }

    result = SDL_RenderClear(m_renderer);

    if (result != 0) {
        throw GameException("Failed to clear renderer: " + NomadString(SDL_GetError()));
    }
}

void Canvas::setOffset(const PointF &offset) {
    m_offset = offset;
}

void Canvas::setOffset(Coord x, Coord y) {
    setOffset({x, y});
}

const PointF& Canvas::getOffset() const {
    return m_offset;
}

void Canvas::renderSprite(const Sprite* sprite, Coord x, Coord y) const {
    if (sprite->getTexture() == nullptr) {
        log::warning("Trying to render a sprite with a null texture");

        return;
    }

    auto& frame = sprite->getFrame();

    const auto& source = frame;

    int left = static_cast<int>(x) + static_cast<int>(sprite->getSource().getLeft());
    int top = static_cast<int>(y) + static_cast<int>(sprite->getSource().getTop());
    const auto destination = Rectangle(
        left,
        top,
        frame.getWidth(),
        frame.getHeight()
    );

    renderTexture(sprite->getTexture(), source, destination);
}

void Canvas::renderTexture(const Texture* texture, const Rectangle& source, const Rectangle& destination) const {
    auto sdl_texture = texture->getSdlTexture();

    auto src_rect = source.toSdlRect();

    auto dst_rect = destination.toSdlRect();

    dst_rect.x += static_cast<int>(m_offset.getX());
    dst_rect.y += static_cast<int>(m_offset.getY());

    int result = SDL_RenderCopy(m_renderer, sdl_texture, &src_rect, &dst_rect);

    if (result != 0) {
        log::error("Failed to render texture: " + NomadString(SDL_GetError()));
    }
}

} // nomad