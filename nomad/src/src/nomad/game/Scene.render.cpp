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
    auto previous_offset = canvas->getOffset();

    auto resolution = m_game->getResolution();

    canvas->setOffset(
        -m_cameraPosition.getX() + resolution.getX() / 2.0,
        -m_cameraPosition.getY() + resolution.getY() / 2.0
    );

    for (const auto& layer : m_layers) {
        // Render tile map
        if (m_tileTexture != nullptr) {
            renderTileMap(canvas, layer);
        }
        // Render entities
        for (const auto entity : layer.entities) {
            entity->render(canvas);
        }
    }

    if (m_game->isDebug()) {
        b2DebugDraw debug_draw;
        createDebugDraw(canvas, &debug_draw);

        for (const auto& layer : m_layers) {
            b2World_Draw(layer.worldId, &debug_draw);
        }
    }

    canvas->setOffset(previous_offset);
}

void Scene::renderTileMap(Canvas* canvas, const Scene::Layer& layer) {
    if (layer.hasGroundTileMap) {
        for (auto y = 0; y < m_tileMapHeight; ++y) {
            for (auto x = 0; x < m_tileMapWidth; ++x) {
                const auto ground_tile_index = layer.groundTileMap[y * m_tileMapWidth + x];

                renderTile(canvas, y, x, ground_tile_index);
            }
        }
    }

    if (layer.hasWallTileMap) {
        for (auto y = 0; y < m_tileMapHeight; ++y) {
            for (auto x = 0; x < m_tileMapWidth; ++x) {
                const auto wall_tile_index = layer.wallTileMap[y * m_tileMapWidth + x];

                renderTile(canvas, y, x, wall_tile_index);
            }
        }
    }
}

void Scene::renderTile(Canvas* canvas, int y, int x, NomadIndex groundTileIndex) {
    if (groundTileIndex > 0) {
        auto& source = m_tiles[groundTileIndex].source;

        Rectangle destination(
            x * m_tileWidth,
            y * m_tileHeight,
            m_tileWidth,
            m_tileHeight
        );

        canvas->renderTexture(m_tileTexture, source, destination);
    }
}

} // nomad