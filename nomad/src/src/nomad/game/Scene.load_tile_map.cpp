//
// Created by jfbil on 2024-11-22.
//

#include "nomad/game/Scene.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include <boost/json.hpp>

#include <algorithm>
#include <fstream>

namespace nomad {

struct TileMapEntity {
    NomadId id;
    NomadString scriptName;
    NomadFloat x;
    NomadFloat y;
    NomadFloat width;
    NomadFloat height;
    NomadString text_id;
};

// Load tile map
void Scene::load_tile_map(const NomadString& file_name, const NomadString& tile_set_texture_name) {
    try {
        log::debug("Loading map '" + file_name + "' with tile texture: '" + tile_set_texture_name + "'");

        // Get texture
//        auto texture_id = m_game->get_resources()->get_textures()->register_texture(tile_set_name);
        auto texture = m_game->get_resources()->get_textures()->get_texture_by_name(tile_set_texture_name);

        if (texture == nullptr) {
            log::error("Texture '" + tile_set_texture_name + "' not found");
            return;
        }

        // Load tile map
        auto tile_map_file_name = m_game->make_resource_path(file_name);

        std::ifstream file(tile_map_file_name);

        if (!file.is_open()) {
            log::error("Failed to open tile map file: " + tile_map_file_name);
            return;
        }

        boost::json::stream_parser parser;

        while (!file.eof()) {
            char buffer[1024];

            file.read(buffer, sizeof(buffer));
            parser.write(buffer, file.gcount());
        }

        auto json = parser.release().as_object();

        // Get tile size
        NomadInteger tile_map_width = json.at("width").as_int64();
        NomadInteger tile_map_height = json.at("height").as_int64();
        NomadInteger tile_width = json.at("tilewidth").as_int64();
        NomadInteger tile_height = json.at("tileheight").as_int64();

        auto layers = json.at("layers").as_array();

        std::array<bool, SCENE_LAYER_COUNT> has_ground_tile_map;
        std::array<std::vector<NomadIndex>, SCENE_LAYER_COUNT> ground_tile_maps;
        std::array<bool, SCENE_LAYER_COUNT> has_wall_tile_map;
        std::array<std::vector<NomadIndex>, SCENE_LAYER_COUNT> wall_tile_maps;
        std::array<std::vector<TileMapEntity>, SCENE_LAYER_COUNT> entities;

        for (auto i = 0; i < SCENE_LAYER_COUNT; ++i) {
            auto& ground_tile_map = ground_tile_maps[i];
            auto& wall_tile_map = wall_tile_maps[i];

            ground_tile_map.resize(tile_map_width * tile_map_height);
            wall_tile_map.resize(tile_map_width * tile_map_height);

            NomadString layer_name = "layer-" + std::to_string(i);

            auto ground_layer_name = layer_name + "-ground";
            auto wall_layer_name = layer_name + "-walls";

            has_ground_tile_map[i] = load_tile_map_layer(ground_layer_name, tile_map_height, tile_map_width, layers, ground_tile_map);
            has_wall_tile_map[i] = load_tile_map_layer(wall_layer_name, tile_map_height, tile_map_width, layers, wall_tile_map);

            // Load entities
            layer_name += "-entities";
            for (auto& layer: layers) {
                if (layer.at("name").as_string().c_str() == layer_name) {
                    auto objects = layer.at("objects").as_array();

                    for (auto& object: objects) {
                        auto id = to_nomad_id(object.at("id").as_int64());
                        auto x = object.at("x").to_number<NomadFloat>();
                        auto y = object.at("y").to_number<NomadFloat>();
                        auto width = object.at("width").to_number<NomadFloat>();
                        auto height = object.at("height").to_number<NomadFloat>();
                        auto properties = object.at("properties").as_array();
                        auto text_id = NomadString();

                        NomadString scriptName;

                        for (auto& property: properties) {
                            auto name = property.at("name").as_string();
                            auto value = property.at("value").as_string();

                            if (name == "scriptName") {
                                scriptName = value;
                            } else if (name == "text") {
                                text_id = value;
                            }
                        }

                        if (scriptName.empty()) {
                            log::error("Entity '" + std::to_string(id) + "' has no script name");
                            continue;
                        }

                        // Tiled used bottom-left as origin
                        y -= height;

                        entities[i].push_back({
                            id,
                            scriptName,
                            x,
                            y,
                            width,
                            height,
                            text_id
                        });
                    }

                    break;
                }
            }
        }

        // TODO: determine start index of tiles
        NomadIndex start_tile_index = 1;

        // Load complete. Initialize the scene.
        set_tile_set(texture, tile_width, tile_height, start_tile_index);

        set_tile_map_size(tile_map_width, tile_map_height);

        m_entities.clear();

        for (auto i = 0; i < SCENE_LAYER_COUNT; ++i) {
            m_layers[i].has_wall_tile_map = has_wall_tile_map[i];
            m_layers[i].has_ground_tile_map = has_ground_tile_map[i];

            auto& ground_tile_map = ground_tile_maps[i];
            auto& wall_tile_map = wall_tile_maps[i];

            auto& layer = m_layers[i];
            layer.ground_tile_map = std::move(ground_tile_map);
            layer.wall_tile_map = std::move(wall_tile_map);

            layer.entities.clear();

            for (auto& entity: entities[i]) {
                create_entity(entity.scriptName, entity.x, entity.y, i, entity.id, entity.text_id);
            }
        }

        // Make sure entities are ready to go by the first frame.
        // Otherwise, the first frame will not contain any entities.
        process_add_remove_entities();

        log::debug("Map loaded");
    } catch (const std::exception& e) {
        log::error("Failed to load tile map: " + std::string(e.what()));
    }
}

bool Scene::load_tile_map_layer(
    const NomadString& ground_layer_name,
    NomadInteger tile_map_height,
    NomadInteger tile_map_width,
    boost::json::array& layers,
    std::vector<NomadIndex>& tile_map
) const {
    for (auto& layer: layers) {
        if (layer.at("name").as_string().c_str() == ground_layer_name) {
            auto data = layer.at("data").as_array();

            for (NomadIndex y = 0; y < tile_map_height; ++y) {
                for (NomadIndex x = 0; x < tile_map_width; ++x) {
                    auto tile_id = data[y * tile_map_width + x].as_int64();
                    tile_map[y * tile_map_width + x] = tile_id;
                }
            }

            return true;
        }
    }

    return false;
}

} // nomad