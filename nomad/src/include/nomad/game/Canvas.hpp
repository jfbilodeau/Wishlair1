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

    [[nodiscard]] Game* get_game() const;

    [[nodiscard]] SDL_Renderer* get_sdl_renderer() const;

    void clear(const Color& color);

    void set_offset(const PointF& offset);
    void set_offset(Coord x, Coord y);
    [[nodiscard]] const PointF& get_offset() const;

    void render_sprite(const Sprite* sprite, Coord x, Coord y) const;
    void render_texture(const Texture* texture, const Rectangle& source, const Rectangle& destination) const;

private:
    Game* m_game;
    SDL_Renderer* m_renderer;
    PointF m_offset;
};

} // nomad

