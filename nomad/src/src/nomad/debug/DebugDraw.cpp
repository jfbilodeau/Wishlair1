//
// Created by Jean-François Bilodeau on 2025-01-05.
//

#include "nomad/debug/DebugDraw.hpp"

#include "nomad/game/Canvas.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/system/FastHeap.hpp"

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

    auto offset = canvas->get_offset();

    std::pmr::vector<SDL_FPoint> temp_vertices(&fast_heap_allocator);

    auto& sdl_color = reinterpret_cast<SDL_Color&>(color);

    for (int i = 0; i < vertex_count; ++i) {
        auto& v = vertices[i];

        temp_vertices.push_back(SDL_FPoint{
            v.x + static_cast<float>(offset.x()),
            v.y + static_cast<float>(offset.y()),
        });
    }

    // Close polygon
    temp_vertices.push_back(SDL_FPoint{
        vertices[0].x + static_cast<float>(offset.x()),
        vertices[0].y  + static_cast<float>(offset.y())
    });

    auto renderer = canvas->get_sdl_renderer();

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

    auto offset = canvas->get_offset();

    std::pmr::vector<SDL_Vertex> temp_vertices(&fast_heap_allocator);

    SDL_Color& sdl_color = reinterpret_cast<SDL_Color&>(color);

    for (int i = 0; i < vertex_count; ++i) {
        auto& v = vertices[i];

        temp_vertices.push_back(SDL_Vertex{
            {
                v.x + static_cast<float>(offset.x()),
                v.y + static_cast<float>(offset.y())
            },
            sdl_color,
            {0, 0}  // Texture coordinates
        });
    }

    SDL_RenderGeometry(
        canvas->get_sdl_renderer(),
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

    auto offset = canvas->get_offset();

    const auto renderer = canvas->get_sdl_renderer();
    const auto& [r, g, b, a] = reinterpret_cast<SDL_Color&>(color);

    constexpr auto vertex_count = 32;

    std::pmr::vector<SDL_FPoint> temp_vertices(&fast_heap_allocator);

    center.x += static_cast<float>(offset.x());
    center.y += static_cast<float>(offset.y());

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

    auto offset = canvas->get_offset();

    auto renderer = canvas->get_sdl_renderer();
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
        p1.x + static_cast<float>(offset.x()), p1.y + static_cast<float>(offset.y()),
        p2.x + static_cast<float>(offset.x()), p2.y + static_cast<float>(offset.y())
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

    auto offset = canvas->get_offset();

    auto renderer = canvas->get_sdl_renderer();

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
        p.x + static_cast<float>(offset.x()),
        p.y + static_cast<float>(offset.y())
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

void create_debug_draw(Canvas* canvas, b2DebugDraw* debug_draw) {
    debug_draw->DrawPolygon = debug_draw_polygon;
    debug_draw->DrawSolidPolygon = debug_draw_solid_polygon;
    debug_draw->DrawCircle = debug_draw_circle;
    debug_draw->DrawSolidCircle = debug_draw_solid_circle;
    debug_draw->DrawSolidCapsule = debug_draw_solid_capsule;
    debug_draw->DrawSegment = debug_draw_segment;
    debug_draw->DrawTransform = debug_draw_transform;
    debug_draw->DrawPoint = debug_draw_point;
    debug_draw->DrawString = debug_draw_string;
    debug_draw->drawingBounds = {
        {0, 0},
        {0, 0}
    };
    debug_draw->useDrawingBounds = false;
    debug_draw->drawShapes = true;
    debug_draw->drawJoints = true;
    debug_draw->drawJointExtras = false;
    debug_draw->drawAABBs = true;
    debug_draw->drawMass = false;
    debug_draw->drawContacts = false;
    debug_draw->drawGraphColors = false;
    debug_draw->drawContactNormals = false;
    debug_draw->drawContactImpulses = false;
    debug_draw->drawFrictionImpulses = false;
    debug_draw->context = canvas;
}

} // namespace nomad