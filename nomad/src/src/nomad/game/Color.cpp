//
// Created by jfbil on 2023-09-12.
//

#include "nomad/game/Color.hpp"
#include "nomad/game/Color.hpp"


namespace nomad {

Color::Color(Rgba rgba) :
    rgba(rgba)
{
}

Color::Color(const Color& other) {
    rgba = other.rgba;
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a):
    r(r),
    g(g),
    b(b),
    a(a)
{
}

SDL_Color Color::to_sdl_color() const {
    return {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

void Color::to_sdl_color(SDL_Color& sdl_color) const {
    sdl_color.r = r;
    sdl_color.g = g;
    sdl_color.b = b;
    sdl_color.a = a;
}

} // nomad