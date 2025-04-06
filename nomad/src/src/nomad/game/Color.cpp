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

Color::Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha):
    red(red),
    green(green),
    blue(blue),
    alpha(alpha)
{
}

SDL_Color Color::toSdlColor() const {
    return {
        .r = red,
        .g = green,
        .b = blue,
        .a = alpha
    };
}

void Color::toSdlColor(SDL_Color& sdlColor) const {
    sdlColor.r = red;
    sdlColor.g = green;
    sdlColor.b = blue;
    sdlColor.a = alpha;
}

} // nomad