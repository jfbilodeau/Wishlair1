//
// Created by jfbil on 2024-12-23.
//

#ifndef NOMAD_ALIGNMENT_HPP
#define NOMAD_ALIGNMENT_HPP

namespace nomad {

enum class HorizontalAlignment {
    Left   = 0b00000001,
    Middle = 0b00000010,
    Right  = 0b00000100
};

enum class VerticalAlignment {
    Top    = 0b00010000,
    Center = 0b00100000,
    Bottom = 0b01000000
};

enum class Alignment {
    TopLeft   = 0b00010001,
    TopMiddle = 0b00010010,
    TopRight  = 0b00010100,

    CenterLeft   = 0b00100001,
    CenterMiddle = 0b00100010,
    CenterRight  = 0b00100100,

    BottomLeft   = 0b01000001,
    BottomMiddle = 0b01000010,
    BottomRight  = 0b01000100
};

HorizontalAlignment get_horizontal_alignment(Alignment alignment);
VerticalAlignment get_vertical_alignment(Alignment alignment);

Alignment get_alignment(HorizontalAlignment horizontal, VerticalAlignment vertical);

} // namespace nomad

#endif //NOMAD_ALIGNMENT_HPP
