//
// Created by jfbil on 2024-11-22.
//

#include "nomad/game/Scene.hpp"

#include "nomad/debug/DebugDraw.hpp"

#include "nomad/game/Canvas.hpp"
#include "nomad/game/Game.hpp"

namespace nomad {

// Render scene
void Scene::render(Canvas* canvas) {
    auto previous_offset = canvas->get_offset();

    auto resolution = m_game->get_resolution();

    canvas->set_offset(
        m_camera_position.x() - resolution.x() / 2.0,
        m_camera_position.y() - resolution.y() / 2.0
    );

    for (const auto& layer : m_layers) {
        // Render tile map
        if (m_tile_texture != nullptr) {
            render_tile_map(canvas, layer);
        }
        // Render entities
        for (const auto entity : layer.entities) {
            entity->render(canvas);
        }
    }

    if (m_game->is_debug()) {
        b2DebugDraw debug_draw;
        create_debug_draw(canvas, &debug_draw);

        for (const auto& layer : m_layers) {
            b2World_Draw(layer.world_id, &debug_draw);
        }
    }

    canvas->set_offset(previous_offset);
}

void Scene::render_tile_map(Canvas* canvas, const Scene::Layer& layer) {
    if (layer.has_ground_tile_map) {
        for (auto y = 0; y < m_tile_map_height; ++y) {
            for (auto x = 0; x < m_tile_map_width; ++x) {
                const auto ground_tile_index = layer.ground_tile_map[y * m_tile_map_width + x];

                render_tile(canvas, y, x, ground_tile_index);
            }
        }
    }

    if (layer.has_wall_tile_map) {
        for (auto y = 0; y < m_tile_map_height; ++y) {
            for (auto x = 0; x < m_tile_map_width; ++x) {
                const auto wall_tile_index = layer.wall_tile_map[y * m_tile_map_width + x];

                render_tile(canvas, y, x, wall_tile_index);
            }
        }
    }
}

void Scene::render_tile(Canvas* canvas, int y, int x, NomadIndex ground_tile_index) {
    if (ground_tile_index > 0) {
        auto& source = m_tiles[ground_tile_index].source;

        Rectangle destination(
            x * m_tile_width,
            y * m_tile_height,
            m_tile_width,
            m_tile_height
        );

        canvas->render_texture(m_tile_texture, source, destination);
    }
}

} // nomad