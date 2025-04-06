//
// Created by jfbil on 2024-11-14.
//

#ifndef NOMAD_LAYER_HPP
#define NOMAD_LAYER_HPP

#include "Entity.hpp"
#include "TileMap.hpp"

namespace nomad {

class Layer {
public:
    Layer();

    void update(Game* game);
    void render(Canvas* canvas);

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);

    void resizeTileMap(NomadIndex width, NomadIndex height);
    [[nodiscard]] NomadIndex getTileLayerWidth() const;
    [[nodiscard]] NomadIndex getTileLayerHeight() const;
    void setTile(NomadIndex x, NomadIndex y, NomadIndex tileId);
    [[nodiscard]] NomadIndex getTile(NomadIndex x, NomadIndex y) const;

private:
    NomadIndex m_tileLayerWidth = 0;
    NomadIndex m_tileLayerHeight = 0;
    std::vector<NomadIndex> m_tileLayer;
    std::vector<Entity*> m_entities;
};

} // namespace nomad

#endif //NOMAD_LAYER_HPP
