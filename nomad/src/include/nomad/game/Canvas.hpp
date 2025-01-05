//
// Created by Jean-François Bilodeau on 2023-06-19.
//

#ifndef NOMAD_CANVAS_HPP
#define NOMAD_CANVAS_HPP

#include "nomad/nomad.hpp"

#include "nomad/game/Color.hpp"
#include "nomad/geometry/Geometry.hpp"

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

    void render_sprite(const Sprite* sprite, Coord x, Coord y);
    void render_texture(const Texture* texture, const Rectangle& source, const Rectangle& destination);

private:
    Game* m_game;
    SDL_Renderer* m_renderer;
};

} // nomad

#endif //NOMAD_CANVAS_HPP
