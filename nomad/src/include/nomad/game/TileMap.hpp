//
// Created by jfbil on 2024-11-01.
//

#ifndef NOMAD_TILEMAP_HPP
#define NOMAD_TILEMAP_HPP

#include "nomad/geometry/PointF.hpp"

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Tile
///////////////////////////////////////////////////////////////////////////////
class Tile {
public:
    Tile() = default;
    explicit Tile(NomadInteger mask);

private:
    NomadInteger m_mask = 0;
};

///////////////////////////////////////////////////////////////////////////////
// TileSet
///////////////////////////////////////////////////////////////////////////////
class TileSet {
public:
    explicit TileSet(NomadIndex tile_count);
    ~TileSet();

private:
    std::vector<Tile> m_tiles;
};

///////////////////////////////////////////////////////////////////////////////
// EntityDefinition
///////////////////////////////////////////////////////////////////////////////
class EntityDefinition {
public:
    explicit EntityDefinition(NomadId entity_id, NomadString script_name);

    [[nodiscard]] NomadId get_entity_id() const;
    [[nodiscard]] const NomadString& get_script_name() const;

private:
    NomadId m_entity_id;
    NomadString m_script_name;
};

///////////////////////////////////////////////////////////////////////////////
// EntitySet
///////////////////////////////////////////////////////////////////////////////
class EntitySet {
public:
    explicit EntitySet(NomadIndex entity_count = 0);

    NomadId register_entity(NomadId entity_id, const NomadString& name);

    [[nodiscard]] const EntityDefinition* get_entity(NomadIndex index) const;

private:
    std::vector<EntityDefinition> m_entities;
};

///////////////////////////////////////////////////////////////////////////////
// TileLayer
///////////////////////////////////////////////////////////////////////////////
class TileLayer {
public:
    TileLayer(NomadIndex width = 0, NomadIndex height = 0, TileSet* tile_set = nullptr);

    [[nodiscard]] NomadIndex get_width() const;
    [[nodiscard]] NomadIndex get_height() const;

    [[nodiscard]] const Tile& get_tile(NomadIndex x, NomadIndex y) const;
    void set_tile(NomadIndex x, NomadIndex y, const Tile& tile);

    [[nodiscard]] const TileSet* get_tile_set() const;

private:
    NomadIndex m_width;
    NomadIndex m_height;
    TileSet* m_tile_set;
    std::vector<Tile> m_tiles;
};

///////////////////////////////////////////////////////////////////////////////
// EntityInstance
///////////////////////////////////////////////////////////////////////////////
class EntityInstance {
public:
    EntityInstance(NomadString entity_id, NomadString  script_name, NomadInteger layer, const PointF& location);

    [[nodiscard]] const NomadString& get_entity_id() const;
    [[nodiscard]] const NomadString& get_script_name() const;
    [[nodiscard]] const PointF& get_location() const;

private:
    NomadString m_entity_id;
    NomadString m_script_name;
    NomadInteger m_layer;
    PointF m_location;
};

///////////////////////////////////////////////////////////////////////////////
// TileMap
///////////////////////////////////////////////////////////////////////////////
class TileMap {
public:
    TileMap(NomadIndex width, NomadIndex height, NomadIndex layer_count);

    [[nodiscard]] NomadIndex get_width() const;
    [[nodiscard]] NomadIndex get_height() const;
    [[nodiscard]] NomadIndex get_layer_count() const;

    [[nodiscard]] const TileLayer* get_layer(NomadIndex index) const;

private:
    NomadIndex m_width, m_height;

    std::vector<TileLayer> m_layers;
};

} // namespace nomad

#endif //NOMAD_TILEMAP_HPP
