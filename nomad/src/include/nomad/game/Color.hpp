//
// Created by jfbil on 2023-09-12.
//

#ifndef NOMAD_COLOR_HPP
#define NOMAD_COLOR_HPP

#include "SDL_pixels.h"

#include <cstdint>

namespace nomad {

using Rgba = std::uint32_t;

class Color {
public:
    Color() = default;
    explicit Color(Rgba rgba);
    Color(const Color& other);
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

    SDL_Color to_sdl_color() const;
    void to_sdl_color(SDL_Color& sdl_color) const;

    union {
        Rgba rgba = 0;
        //SDL_Color sdl_color;
        struct {
            Uint8 a;
            Uint8 b;
            Uint8 g;
            Uint8 r;
        };
    };
};

static bool operator==(const Color& lhs, const Color& rhs) {
    return lhs.rgba == rhs.rgba;
}

static bool operator!=(const Color& lhs, const Color& rhs) {
    return !(lhs == rhs);
}

namespace Colors {
    const Color Black = Color(0, 0, 0);
    const Color White = Color(255, 255, 255);
    const Color Red = Color(255, 0, 0);
    const Color Green = Color(0, 255, 0);
    const Color Blue = Color(0, 0, 255);
    const Color Yellow = Color(255, 255, 0);
    const Color Magenta = Color(255, 0, 255);
    const Color Cyan = Color(0, 255, 255);
    const Color Transparent = Color(0, 0, 0, 0);
} // Colors

} // nomad

#endif //NOMAD_COLOR_HPP
