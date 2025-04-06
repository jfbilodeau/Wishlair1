//
// Created by jfbil on 2024-11-14.
//

#include "nomad/game/Layer.hpp"

namespace nomad {

Layer::Layer():
    m_tileLayer()
{
}

void Layer::update(Game* game) {
//    for (auto entity : m_entities) {
//        entity->update(this);
//    }
}

void Layer::render(Canvas* canvas) {
    for (auto entity : m_entities) {
        entity->render(canvas);
    }
}

void Layer::addEntity(Entity* entity) {
    m_entities.push_back(entity);
}

void Layer::removeEntity(Entity* entity) {
    const auto it = std::find(m_entities.begin(), m_entities.end(), entity);

    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}

void Layer::resizeTileMap(NomadIndex width, NomadIndex height) {
    // Create new tile layer. Copy old tiles to new layer and move new layer to m_tile_layer.
    std::vector<NomadIndex> newTileLayer(width * height);

    for (NomadIndex y = 0; y < height; ++y) {
        for (NomadIndex x = 0; x < width; ++x) {
            if (x < m_tileLayerWidth && y < m_tileLayerHeight) {
                newTileLayer[y * width + x] = m_tileLayer[y * m_tileLayerWidth + x];
            } else {
                newTileLayer[y * width + x] = 0;
            }
        }
    }

    m_tileLayer = std::move(newTileLayer);

    m_tileLayerWidth = width;
    m_tileLayerHeight = height;
}

NomadIndex Layer::getTileLayerWidth() const {
    return m_tileLayerWidth;
}

NomadIndex Layer::getTileLayerHeight() const {
    return m_tileLayerHeight;
}

void Layer::setTile(NomadIndex x, NomadIndex y, NomadIndex tileId) {
    if (x >= m_tileLayerWidth || y >= m_tileLayerHeight) {
        return;
    }

    m_tileLayer[y * m_tileLayerWidth + x] = tileId;
}

NomadIndex Layer::getTile(NomadIndex x, NomadIndex y) const {
    if (x >= m_tileLayerWidth || y >= m_tileLayerHeight) {
        return 0;
    }

    return m_tileLayer[y * m_tileLayerWidth + x];
}

} // namespace nomad