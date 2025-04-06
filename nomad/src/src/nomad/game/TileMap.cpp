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
TileSet::TileSet(NomadIndex tileCount) {
    m_tiles.resize(tileCount);
}

TileSet::~TileSet() {

}

///////////////////////////////////////////////////////////////////////////////
// EntityDefinition
EntityDefinition::EntityDefinition(NomadId entityId, NomadString scriptName):
    m_entityId(entityId),
    m_scriptName(std::move(scriptName))
{}

NomadId EntityDefinition::getEntityId() const {
    return m_entityId;
}

const NomadString& EntityDefinition::getScriptName() const {
    return m_scriptName;
}

///////////////////////////////////////////////////////////////////////////////
// EntitySet
EntitySet::EntitySet(NomadIndex entityCount) {
    m_entities.reserve(entityCount);
}

NomadId EntitySet::registerEntity(NomadId entityId, const NomadString& name) {
    auto id = to_nomad_id(m_entities.size());

    m_entities.emplace_back(
        entityId,
        name
    );

    return id;
}

const EntityDefinition* EntitySet::getEntity(NomadIndex index) const {
    return &m_entities[index];
}

///////////////////////////////////////////////////////////////////////////////
// TileLayer
TileLayer::TileLayer(NomadIndex width, NomadIndex height, TileSet* tileSet):
    m_width(width),
    m_height(height),
    m_tileSet(tileSet)
{
    m_tiles.resize(width * height);
}

NomadIndex TileLayer::getWidth() const {
    return m_width;
}

NomadIndex TileLayer::getHeight() const {
    return m_height;
}

const Tile& TileLayer::getTile(NomadIndex x, NomadIndex y) const {
    if (x >= m_width || y >= m_height) {
        return m_tiles[y * m_width + x];
    }

    // Return default 'void' tile
    return m_tiles[0];
}

void TileLayer::setTile(NomadIndex x, NomadIndex y, const Tile& tile) {
    if (x >= m_width || y >= m_height) {
        return;
    }

    m_tiles[y * m_width + x] = tile;
}

const TileSet* TileLayer::getTileSet() const {
    return m_tileSet;
}

///////////////////////////////////////////////////////////////////////////////
// EntityInstance
EntityInstance::EntityInstance(
    NomadString entityId,
    NomadString  scriptName,
    NomadInteger layer,
    const PointF& location
):
    m_entityId(std::move(entityId)),
    m_scriptName(std::move(scriptName)),
    m_layer(layer),
    m_location(location)
{
}

const NomadString& EntityInstance::getEntityId() const {
    return m_entityId;
}

const NomadString& EntityInstance::getScriptName() const {
    return m_scriptName;
}

const PointF& EntityInstance::getLocation() const {
    return m_location;
}

///////////////////////////////////////////////////////////////////////////////
// TileMap
TileMap::TileMap(NomadIndex width, NomadIndex height, NomadIndex layerCount):
    m_width(width),
    m_height(height)
{
    m_layers.reserve(layerCount);
}
} // namespace nomad