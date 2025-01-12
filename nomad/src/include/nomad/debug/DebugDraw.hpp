//
// Created by jfbil on 2024-12-31.
//

#ifndef NOMAD_DEBUGDRAW_HPP
#define NOMAD_DEBUGDRAW_HPP

#include "box2d/box2d.h"

namespace nomad {

// Forward declarations
class Canvas;

void create_debug_draw(Canvas* canvas, b2DebugDraw* debug_draw);

} // namespace nomad

#endif //NOMAD_DEBUGDRAW_HPP
