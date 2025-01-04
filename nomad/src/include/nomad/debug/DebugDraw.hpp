//
// Created by jfbil on 2024-12-31.
//

#ifndef NOMAD_DEBUGDRAW_HPP
#define NOMAD_DEBUGDRAW_HPP

#include "box2d/box2d.h"

namespace nomad {

// Forward declarations
class Canvas;

class DebugDraw : public b2DebugDraw {
public:
    DebugDraw(Canvas* canvas);

    

private:
    Canvas* m_canvas;
};

} // namespace nomad

#endif //NOMAD_DEBUGDRAW_HPP
