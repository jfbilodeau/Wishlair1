//
// Created by Jean-François Bilodeau on 2023-06-19.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/game/Color.hpp"
#include "nomad/geometry/Geometry.hpp"
#include "nomad/geometry/PointF.hpp"

// Forward declarations
struct SDL_Renderer;
struct SDL_Surface;

namespace nomad {

// Forward declarations
class Game;
class Rectangle;
class Sprite;
class Texture;

class Canvas {
public:
    explicit Canvas(Game* game, SDL_Renderer* renderer);
    Canvas(const Canvas& other) = delete;
    ~Canvas() = default;

    [[nodiscard]] Game* getGame() const;

    [[nodiscard]] SDL_Renderer* getSdlRenderer() const;

    void clear(const Color& color);

    void setOffset(const PointF& offset);
    void setOffset(Coord x, Coord y);
    [[nodiscard]] const PointF& getOffset() const;

    void renderSprite(const Sprite* sprite, Coord x, Coord y) const;
    void renderTexture(const Texture* texture, const Rectangle& source, const Rectangle& destination) const;

private:
    Game* m_game;
    SDL_Renderer* m_renderer;
    PointF m_offset;
};

} // nomad

