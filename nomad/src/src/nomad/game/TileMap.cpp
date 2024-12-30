//
// Created by jfbil on 2024-11-01.
//

#include <utility>

#include "nomad/game/TileMap.hpp"

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Tile
Tile::Tile(NomadInteger mask):
    m_mask(mask)
{}

///////////////////////////////////////////////////////////////////////////////
// TileSet
TileSet::TileSet(NomadIndex tile_count) {
    m_tiles.resize(tile_count);
}

TileSet::~TileSet() {

}

///////////////////////////////////////////////////////////////////////////////
// EntityDefinition
EntityDefinition::EntityDefinition(NomadId entity_id, NomadString script_name):
    m_entity_id(entity_id),
    m_script_name(std::move(script_name))
{}

NomadId EntityDefinition::get_entity_id() const {
    return m_entity_id;
}

const NomadString& EntityDefinition::get_script_name() const {
    return m_script_name;
}

///////////////////////////////////////////////////////////////////////////////
// EntitySet
EntitySet::EntitySet(NomadIndex entity_count) {
    m_entities.reserve(entity_count);
}

NomadId EntitySet::register_entity(NomadId entity_id, const NomadString& name) {
    auto id = to_nomad_id(m_entities.size());

    m_entities.emplace_back(
        entity_id,
        name
    );

    return id;
}

const EntityDefinition* EntitySet::get_entity(NomadIndex index) const {
    return &m_entities[index];
}

///////////////////////////////////////////////////////////////////////////////
// TileLayer
TileLayer::TileLayer(NomadIndex width, NomadIndex height, TileSet* tile_set):
    m_width(width),
    m_height(height),
    m_tile_set(tile_set)
{
    m_tiles.resize(width * height);
}

NomadIndex TileLayer::get_width() const {
    return m_width;
}

NomadIndex TileLayer::get_height() const {
    return m_height;
}

const Tile& TileLayer::get_tile(NomadIndex x, NomadIndex y) const {
    if (x >= m_width || y >= m_height) {
        return m_tiles[y * m_width + x];
    }

    // Return default 'void' tile
    return m_tiles[0];
}

void TileLayer::set_tile(NomadIndex x, NomadIndex y, const Tile& tile) {
    if (x >= m_width || y >= m_height) {
        return;
    }

    m_tiles[y * m_width + x] = tile;
}

const TileSet* TileLayer::get_tile_set() const {
    return m_tile_set;
}

///////////////////////////////////////////////////////////////////////////////
// EntityInstance
EntityInstance::EntityInstance(
    NomadString entity_id,
    NomadString  script_name,
    NomadInteger layer,
    const PointF& location
):
    m_entity_id(std::move(entity_id)),
    m_script_name(std::move(script_name)),
    m_layer(layer),
    m_location(location)
{
}

const NomadString& EntityInstance::get_entity_id() const {
    return m_entity_id;
}

const NomadString& EntityInstance::get_script_name() const {
    return m_script_name;
}

const PointF& EntityInstance::get_location() const {
    return m_location;
}

///////////////////////////////////////////////////////////////////////////////
// TileMap
TileMap::TileMap(NomadIndex width, NomadIndex height, NomadIndex layer_count):
    m_width(width),
    m_height(height)
{
    m_layers.reserve(layer_count);
}
} // namespace nomad