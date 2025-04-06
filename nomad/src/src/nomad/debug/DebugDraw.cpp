//
// Created by Jean-François Bilodeau on 2025-01-05.
//

#include "nomad/debug/DebugDraw.hpp"

#include "nomad/game/Canvas.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/system/TempHeap.hpp"

#include <SDL.h>

namespace nomad {

namespace {

void debug_draw_polygon(
    const b2Vec2* vertices,
    int vertex_count,
    b2HexColor color,
    void* context
) {
    auto canvas = static_cast<Canvas*>(context);

    auto offset = canvas->getOffset();

    auto temp_vertices = createTempVector<SDL_FPoint>();

    auto& sdl_color = reinterpret_cast<SDL_Color&>(color);

    for (int i = 0; i < vertex_count; ++i) {
        auto& v = vertices[i];

        temp_vertices.push_back(SDL_FPoint{
            v.x + static_cast<float>(offset.getX()),
            v.y + static_cast<float>(offset.getY()),
        });
    }

    // Close polygon
    temp_vertices.push_back(SDL_FPoint{
        vertices[0].x + static_cast<float>(offset.getX()),
        vertices[0].y  + static_cast<float>(offset.getY())
    });

    auto renderer = canvas->getSdlRenderer();

    SDL_SetRenderDrawColor(
        renderer,
        sdl_color.r,
        sdl_color.g,
        sdl_color.b,
        sdl_color.a
    );

    SDL_RenderDrawLinesF(
        renderer,
        temp_vertices.data(),
        temp_vertices.size()
    );
}

void debug_draw_solid_polygon(
    b2Transform transform,
    const b2Vec2* vertices,
    int vertex_count,
    float radius,
    b2HexColor color,
    void* context
) {
    auto canvas = static_cast<Canvas*>(context);

    auto offset = canvas->getOffset();

    auto temp_vertices = createTempVector<SDL_Vertex>();

    SDL_Color& sdl_color = reinterpret_cast<SDL_Color&>(color);

    for (int i = 0; i < vertex_count; ++i) {
        auto& v = vertices[i];

        temp_vertices.push_back(SDL_Vertex{
            {
                v.x + static_cast<float>(offset.getX()),
                v.y + static_cast<float>(offset.getY())
            },
            sdl_color,
            {0, 0}  // Texture coordinates
        });
    }

    SDL_RenderGeometry(
        canvas->getSdlRenderer(),
        nullptr, // No texture
        temp_vertices.data(),
        temp_vertices.size(),
        nullptr, // No indices
        0 // No indices count
    );
}

void debug_draw_circle(
    b2Vec2 center,
    float radius,
    b2HexColor color,
    void* context
) {
    const auto canvas = static_cast<Canvas*>(context);

    auto offset = canvas->getOffset();

    const auto renderer = canvas->getSdlRenderer();
    const auto& [r, g, b, a] = reinterpret_cast<SDL_Color&>(color);

    constexpr auto vertex_count = 32;

    auto temp_vertices = createTempVector<SDL_FPoint>();

    center.x += static_cast<float>(offset.getX());
    center.y += static_cast<float>(offset.getY());

    for (int i = 0; i <= vertex_count; ++i) {  // Using '<=' (+1) to close the circle
        const auto angle = 2.0f * static_cast<float>(M_PI) * i / vertex_count;
        const auto x = center.x + radius * cosf(angle);
        const auto y = center.y + radius * sinf(angle);
        temp_vertices.push_back(SDL_FPoint{x, y});
    }

    SDL_SetRenderDrawColor(
        renderer,
        r,
        g,
        b,
        a
    );

    SDL_RenderDrawLinesF(
        renderer,
        temp_vertices.data(),
        temp_vertices.size()
    );
}

void debug_draw_solid_circle(
    b2Transform transform,
    float radius,
    b2HexColor color,
    void* context
) {
    // Temporary cheat
    debug_draw_circle(transform.p, radius, color, context);
}

void debug_draw_solid_capsule(
    b2Vec2 p1,
    b2Vec2 p2,
    float radius,
    b2HexColor color,
    void* context
) {
    log::warning("debug_draw_solid_capsule() called but Box2D capsule shapes are not used.");
}

void debug_draw_segment(
    b2Vec2 p1,
    b2Vec2 p2,
    b2HexColor color,
    void* context
) {
    auto canvas = static_cast<Canvas*>(context);

    auto offset = canvas->getOffset();

    auto renderer = canvas->getSdlRenderer();
    auto& sdl_color = reinterpret_cast<SDL_Color&>(color);

    SDL_SetRenderDrawColor(
        renderer,
        sdl_color.r,
        sdl_color.g,
        sdl_color.b,
        sdl_color.a
    );

    SDL_RenderDrawLineF(
        renderer,
        p1.x + static_cast<float>(offset.getX()), p1.y + static_cast<float>(offset.getY()),
        p2.x + static_cast<float>(offset.getX()), p2.y + static_cast<float>(offset.getY())
    );
}

void debug_draw_transform(
    b2Transform transform,
    void* context
) {
    log::warning("debug_draw_transform() not implemented");
}

void debug_draw_point(
    b2Vec2 p,
    float size,
    b2HexColor color,
    void* context
) {
    auto canvas = static_cast<Canvas*>(context);

    auto offset = canvas->getOffset();

    auto renderer = canvas->getSdlRenderer();

    auto& sdl_color = reinterpret_cast<SDL_Color&>(color);

    SDL_SetRenderDrawColor(
        renderer,
        sdl_color.r,
        sdl_color.g,
        sdl_color.b,
        sdl_color.a
    );

    SDL_RenderDrawPointF(
        renderer,
        p.x + static_cast<float>(offset.getX()),
        p.y + static_cast<float>(offset.getY())
    );
}

void debug_draw_string(
    b2Vec2 p,
    const char* s,
    b2HexColor color,
    void* context
) {
    // Ignore...
}

void bounds(
    b2AABB aabb,
    void* context
) {
    log::warning("bounds() not implemented");
}

} // namespace (anonymous)

void createDebugDraw(Canvas* canvas, b2DebugDraw* debugDraw) {
    debugDraw->DrawPolygon = debug_draw_polygon;
    debugDraw->DrawSolidPolygon = debug_draw_solid_polygon;
    debugDraw->DrawCircle = debug_draw_circle;
    debugDraw->DrawSolidCircle = debug_draw_solid_circle;
    debugDraw->DrawSolidCapsule = debug_draw_solid_capsule;
    debugDraw->DrawSegment = debug_draw_segment;
    debugDraw->DrawTransform = debug_draw_transform;
    debugDraw->DrawPoint = debug_draw_point;
    debugDraw->DrawString = debug_draw_string;
    debugDraw->drawingBounds = {
        {0, 0},
        {0, 0}
    };
    debugDraw->useDrawingBounds = false;
    debugDraw->drawShapes = true;
    debugDraw->drawJoints = true;
    debugDraw->drawJointExtras = false;
    debugDraw->drawAABBs = true;
    debugDraw->drawMass = false;
    debugDraw->drawContacts = false;
    debugDraw->drawGraphColors = false;
    debugDraw->drawContactNormals = false;
    debugDraw->drawContactImpulses = false;
    debugDraw->drawFrictionImpulses = false;
    debugDraw->context = canvas;
}

} // namespace nomad