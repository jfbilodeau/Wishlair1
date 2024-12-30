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

    void add_entity(Entity* entity);
    void remove_entity(Entity* entity);

    void resize_tile_map(NomadIndex width, NomadIndex height);
    [[nodiscard]] NomadIndex get_tile_layer_width() const;
    [[nodiscard]] NomadIndex get_tile_layer_height() const;
    void set_tile(NomadIndex x, NomadIndex y, NomadIndex tile_id);
    [[nodiscard]] NomadIndex get_tile(NomadIndex x, NomadIndex y) const;

private:
    NomadIndex m_tile_layer_width = 0;
    NomadIndex m_tile_layer_height = 0;
    std::vector<NomadIndex> m_tile_layer;
    std::vector<Entity*> m_entities;
};

} // namespace nomad

#endif //NOMAD_LAYER_HPP
