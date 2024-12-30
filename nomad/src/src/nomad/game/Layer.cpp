//
// Created by jfbil on 2024-11-14.
//

#include "nomad/game/Layer.hpp"

namespace nomad {

Layer::Layer():
    m_tile_layer()
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

void Layer::add_entity(Entity* entity) {
    m_entities.push_back(entity);
}

void Layer::remove_entity(Entity* entity) {
    const auto it = std::find(m_entities.begin(), m_entities.end(), entity);

    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}

void Layer::resize_tile_map(NomadIndex width, NomadIndex height) {
    // Create new tile layer. Copy old tiles to new layer and move new layer to m_tile_layer.
    std::vector<NomadIndex> new_tile_layer(width * height);

    for (NomadIndex y = 0; y < height; ++y) {
        for (NomadIndex x = 0; x < width; ++x) {
            if (x < m_tile_layer_width && y < m_tile_layer_height) {
                new_tile_layer[y * width + x] = m_tile_layer[y * m_tile_layer_width + x];
            } else {
                new_tile_layer[y * width + x] = 0;
            }
        }
    }

    m_tile_layer = std::move(new_tile_layer);

    m_tile_layer_width = width;
    m_tile_layer_height = height;
}

NomadIndex Layer::get_tile_layer_width() const {
    return m_tile_layer_width;
}

NomadIndex Layer::get_tile_layer_height() const {
    return m_tile_layer_height;
}

void Layer::set_tile(NomadIndex x, NomadIndex y, NomadIndex tile_id) {
    if (x >= m_tile_layer_width || y >= m_tile_layer_height) {
        return;
    }

    m_tile_layer[y * m_tile_layer_width + x] = tile_id;
}

NomadIndex Layer::get_tile(NomadIndex x, NomadIndex y) const {
    if (x >= m_tile_layer_width || y >= m_tile_layer_height) {
        return 0;
    }

    return m_tile_layer[y * m_tile_layer_width + x];
}

} // namespace nomad