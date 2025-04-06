//
// Created by jfbil on 2023-09-12.
//

#pragma once

#include "SDL_pixels.h"

#include <cstdint>

namespace nomad {

using Rgba = std::uint32_t;

class Color {
public:
    Color() = default;
    explicit Color(Rgba rgba);
    Color(const Color& other);
    Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255);

    SDL_Color toSdlColor() const;
    void toSdlColor(SDL_Color& sdlColor) const;

    union {
        Rgba rgba = 0;
        struct {
            Uint8 alpha;
            Uint8 blue;
            Uint8 green;
            Uint8 red;
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

