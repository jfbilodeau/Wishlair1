//
// Created by jfbilodeau on 23-06-14.
//

#include "nomad/game/Scene.hpp"

#include "nomad/geometry/Rectangle.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Texture.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/script/Runtime.hpp"

#include <algorithm>

namespace nomad {

Scene::Scene(Game* game):
    m_game(game),
    m_variables(game->get_scene_variables())
{
    // Force load of tileset texture
//    auto _ = game->get_resources()->get_textures()->get_texture_by_name("world/tileset.png");
//    load_tile_map("world/test-1.tmj", "world/tileset.png");

    // Create Box2D worlds
    for (NomadId id = 0; id < SCENE_LAYER_COUNT; ++id) {
        auto& layer = m_layers[id];

        layer.id = id;

        b2WorldDef world_def = b2DefaultWorldDef();

        layer.world_id = b2CreateWorld(&world_def);
    }
}

Scene::~Scene() {
    for (auto& layer : m_layers) {
        b2DestroyWorld(layer.world_id);
    }
}

Game* Scene::get_game() const {
    return m_game;
}

void Scene::set_game(Game* game) {
    this->m_game = game;
}

void Scene::set_name(const NomadString& name) {
    m_name = name;
}

NomadString Scene::get_name() const {
    return m_name;
}

void Scene::set_z(NomadInteger z) {
    this->m_z = z;
}

NomadInteger Scene::get_z() const {
    return m_z;
}

void Scene::process_input_event(const InputEvent& event) {
    NomadString action_name;

    if (m_action_manager.get_action_name_for_input(event.code, action_name)) {
        for (auto& map: m_action_mapping) {
            if (
                map.name == action_name &&
                map.action == event.action
            ) {
                map.pressed = event.action == InputAction::Pressed;
                map.released = event.action == InputAction::Released;
                map.held = map.pressed;

                const auto entity = get_entity_by_id(map.entity_id);

                m_game->execute_script_in_new_context(map.script_id, this, entity);
            }
        }
    }
}

void Scene::set_variable_value(NomadId variable_id, const ScriptValue& value) {
    m_variables.set_variable_value(variable_id, value);
}

void Scene::get_variable_value(NomadId variable_id, ScriptValue& value) const {
    m_variables.get_variable_value(variable_id, value);
}

void Scene::create_entity(
    const NomadString& init_script_name,
    NomadFloat x,
    NomadFloat y,
    NomadInteger layer,
    NomadId id,
    const NomadString& text
) {
    const auto init_script_id = m_game->get_script_id(init_script_name);

    if (init_script_id == NOMAD_INVALID_ID) {
        log::error("Script for entity '" + init_script_name + "' not found");
        return; // skip!;
    }

    m_added_entities.push_back({
        init_script_id,
        x,
        y,
        layer,
        id,
        text
    });
}

void Scene::remove_entity(Entity* entity) {
    // Make sure entity is not already in the list of removed entities
    const auto it = std::find(m_removed_entities.begin(), m_removed_entities.end(), entity);

    if (it == m_removed_entities.end()) {
        m_removed_entities.push_back(entity);
    }
}

Entity* Scene::get_entity_by_id(NomadId id) const {
    for (const auto entity : m_entities) {
        if (entity->get_id() == id) {
            return entity;
        }
    }

    return nullptr;
}

Entity* Scene::get_entity_by_name(const NomadString& name) const {
    for (const auto entity : m_entities) {
        if (entity->get_name() == name) {
            return entity;
        }
    }

    return nullptr;
}

void Scene::get_entities_by_name(const NomadString& name, EntityList& entities) const {
    for (const auto entity : m_entities) {
        if (entity->get_name() == name) {
            entities.push_back(entity);
        }
    }
}

void Scene::add_event(const NomadString& name, NomadId script_id) {
    m_events.add_event(name, script_id);
}

void Scene::remove_event(const NomadString& name) {
    m_events.remove_event(name);
}

void Scene::load_action_mapping(const NomadString& mapping_name) {
    m_action_manager.load_mapping(m_game, mapping_name);
}

void Scene::save_action_mapping(const NomadString& mapping_name) {
    m_action_manager.save_mapping(m_game, mapping_name);
}

void Scene::reset_action_mapping(const NomadString& mapping_name) {
    m_action_manager.reset_mapping_to_defaults(m_game, mapping_name);
}

void Scene::add_action_pressed(const NomadString& action_name, NomadId script_id, NomadId entity_id) {
    auto mapping = get_action_mapping(action_name, InputAction::Pressed, entity_id);

    if (mapping) {
        // Update mapping:
        mapping->script_id = script_id;
    } else {
        m_action_mapping.emplace_back(
            ActionMapping{
                action_name,
                InputAction::Pressed,
                script_id,
                entity_id
            }
        );
    }
}

void Scene::add_action_released(const NomadString& action_name, NomadId script_id, NomadId entity_id) {
    auto mapping = get_action_mapping(action_name, InputAction::Released, entity_id);

    if (mapping) {
        // Update mapping:
        mapping->script_id = script_id;
    } else {
        m_action_mapping.emplace_back(
            ActionMapping{
                action_name,
                InputAction::Released,
                script_id,
                entity_id
            }
        );
    }
}

void Scene::remove_action_pressed(const NomadString& action_name, NomadId entity_id) {
    const auto i = std::find_if(m_action_mapping.begin(), m_action_mapping.end(), [action_name, entity_id](const ActionMapping& mapping) {
        return mapping.name == action_name && mapping.action == InputAction::Pressed && mapping.entity_id == entity_id;
    });

    m_action_mapping.erase(i);
}

void Scene::remove_action_released(const NomadString& action_name, NomadId entity_id) {
    const auto i = std::find_if(m_action_mapping.begin(), m_action_mapping.end(), [action_name, entity_id](const ActionMapping& mapping) {
        return mapping.name == action_name && mapping.action == InputAction::Pressed && mapping.entity_id == entity_id;
    });

    m_action_mapping.erase(i);
}

NomadId Scene::get_next_entity_id() {
    const auto start_id = m_entity_id_counter;

    if (m_entities.size() >= NOMAD_ID_MAX - NOMAD_ID_MIN) {
        // We're already at the max number of entities.
        return NOMAD_INVALID_ID;
    }

    while (true) {
        const auto entity = get_entity_by_id(m_entity_id_counter);

        if (entity == nullptr) {
            return m_entity_id_counter;
        }

        m_entity_id_counter++;

        if (m_entity_id_counter == NOMAD_ID_MAX) {
            m_entity_id_counter = NOMAD_ID_MIN;
        }

        if (m_entity_id_counter == start_id) {
            return NOMAD_INVALID_ID;
        }
    }
}

Scene::ActionMapping* Scene::get_action_mapping(const NomadString& name, InputAction type, NomadId entity_id) {
    for (auto& mapping : m_action_mapping) {
        if (mapping.name == name && mapping.action == type && mapping.entity_id == entity_id) {
            return &mapping;
        }
    }

    return nullptr;
}

void Scene::set_tile_set(const Texture* texture, NomadInteger tile_width, NomadInteger tile_height, NomadIndex first_tile_index) {
    m_tile_texture = texture;
//    m_tile_texture_width = texture->get_width();
//    m_tile_texture_height = texture->get_height();
    auto horizontal_tile_count = texture->get_width() / tile_width;
    auto vertical_tile_count = texture->get_height() / tile_height;
    m_tile_width = tile_width;
    m_tile_height = tile_height;
    m_tile_count = horizontal_tile_count * vertical_tile_count + first_tile_index; // +1 to accommodate for tile '0' which is not part of a tileset in Tiled
    m_tiles.resize(m_tile_count);

    // Reset tile masks to 0
    for (auto& tile : m_tiles) {
        tile.mask = 0;
    }

    // Init source coordinates for each tile
    for (NomadIndex y = 0; y < vertical_tile_count; ++y) {
        for (NomadIndex x = 0; x < horizontal_tile_count; ++x) {
            NomadIndex tile_index = y * vertical_tile_count + x + first_tile_index;
            m_tiles[tile_index].source = Rectangle(
                x * tile_width,
                y * tile_height,
                tile_width,
                tile_height
            );
        }
    }
}

void Scene::set_tile_mask(NomadIndex tile_index, NomadInteger tile_mask) {
    if (tile_index >= m_tiles.size()) {
        return;
    }

    m_tiles[tile_index].mask = tile_mask;
}

NomadInteger Scene::get_tile_mask(NomadIndex tile_index) const {
    if (tile_index >= m_tiles.size()) {
        return 0;
    }

    return m_tiles[tile_index].mask;
}

void Scene::set_tile_map_size(NomadInteger width, NomadInteger height) {
    for (auto& layer : m_layers) {
        std::vector<NomadIndex> new_ground_tile_layer(width * height);
        std::vector<NomadIndex> new_wall_tile_layer(width * height);

        for (NomadIndex y = 0; y < height; ++y) {
            for (NomadIndex x = 0; x < width; ++x) {
                if (x < m_tile_map_width && y < m_tile_map_height) {
                    new_ground_tile_layer[y * width + x] = layer.ground_tile_map[y * m_tile_map_width + x];
                    new_wall_tile_layer[y * width + x] = layer.wall_tile_map[y * m_tile_map_width + x];
                } else {
                    new_ground_tile_layer[y * width + x] = 0;
                    new_wall_tile_layer[y * width + x] = 0;
                }
            }
        }

        layer.ground_tile_map = std::move(new_ground_tile_layer);
        layer.wall_tile_map = std::move(new_wall_tile_layer);
    }

    m_tile_map_width = width;
    m_tile_map_height = height;
}

NomadInteger Scene::get_tile_map_width() const {
    return m_tile_map_width;
}

NomadInteger Scene::get_tile_map_height() const {
    return m_tile_map_height;
}

void Scene::set_ground_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index) {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    if (x >= m_tile_map_width || y >= m_tile_map_height) {
        return;
    }

    tile_layer.ground_tile_map[y * m_tile_map_width + x] = tile_index;
}

void Scene::set_wall_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index) {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    if (x >= m_tile_map_width || y >= m_tile_map_height) {
        return;
    }

    tile_layer.ground_tile_map[y * m_tile_map_width + x] = tile_index;
}

void Scene::set_wall_mask(NomadInteger mask) {
    m_wall_filter.categoryBits = mask;

    for (auto& layer : m_layers) {
        for (auto& wall : layer.walls) {
            b2ShapeId shape_id;
            b2Body_GetShapes(wall, &shape_id, 1);
            b2Shape_SetFilter(shape_id, m_wall_filter);
        }
    }
}

NomadInteger Scene::get_wall_mask() const {
    return m_wall_filter.categoryBits;
}

NomadIndex Scene::get_ground_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (
        layer >= m_layers.size() ||
        x < 0 ||
        x >= m_tile_map_width ||
        y < 0 ||
        y >= m_tile_map_height
    ) {
        return 0;
    }

    return m_layers[layer].ground_tile_map[y * m_tile_map_width + x];
}

NomadIndex Scene::get_wall_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (
        layer >= m_layers.size() ||
        x < 0 ||
        x >= m_tile_map_width ||
        y < 0 ||
        y >= m_tile_map_height
    ) {
        return 0;
    }

    return m_layers[layer].ground_tile_map[y * m_tile_map_width + x];
}

NomadInteger Scene::get_tile_mask(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    auto tile_index = get_ground_tile_index(layer, x, y);

    if (tile_index >= m_tiles.size()) {
        return 0;
    }

    return m_tiles[tile_index].mask;
}

void Scene::process_tile_at(NomadIndex layer, const Rectangle& rectangle, TileCallback callback) const {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    auto tileLeft = std::max(rectangle.get_left() / m_tile_width, (NomadInteger)0);
    auto tileRight = std::min(rectangle.get_right() / m_tile_width, m_tile_map_width - 1);
    auto tileTop = std::max(rectangle.get_top() / m_tile_height, (NomadInteger)0);
    auto tileBottom = std::min(rectangle.get_bottom() / m_tile_height, m_tile_map_height - 1);

    for (NomadInteger y = tileTop; y <= tileBottom; ++y) {
        for (NomadInteger x = tileLeft; x <= tileRight; ++x) {
            auto tile_id = tile_layer.ground_tile_map[y * m_tile_map_width + x];
            auto tile_mask = m_tiles[tile_id].mask;

            TileInformation information {
                x,
                y,
                tile_id,
                tile_mask
            };

            callback(information);
        }
    }
}

NomadInteger Scene::get_mask_at_entity(const Entity* entity) const {
    return get_mask_at_entity(entity, entity->get_location());
}

NomadInteger Scene::get_mask_at_entity(const Entity* entity, const PointF& location) const {
    auto layer = entity->get_layer();

    auto body_shape = entity->get_body_shape();

    if (body_shape == BodyShape::Rectangle) {
        auto width = entity->get_body_width();
        auto height = entity->get_body_height();
        auto x = location.x() - width / 2;
        auto y = location.y() - height / 2;

        RectangleF rectangle(x, y, width, height);

        return get_mask_in_rectangle(layer, rectangle, entity);
    } else if (body_shape == BodyShape::Circle) {
        auto x = location.x();
        auto y = location.y();
        auto radius = entity->get_body_radius();

        CircleF circle(x, y, radius);

        return get_mask_in_circle(layer, circle, entity);
    } else if (body_shape == BodyShape::None) {
        // No body shape, so no mask
        return 0;
    } else {
        log::warning("Unexpected body shape: " + std::to_string(static_cast<int>(body_shape)));
        return 0;
    }
}

NomadInteger Scene::get_mask_from_entities_at(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const {
//    if (layer >= m_layers.size()) {
//        return 0;
//    }
//
//    NomadInteger mask = 0;
//
//    for (auto entity : m_layers[layer].entities) {
//        if (entity == exclude) {
//            continue;
//        }
//
//        if (entity->is_touching(rectangle)) {
//            mask |= entity->get_mask();
//        }
//    }
//
//    return mask;
    return 0;
}

NomadInteger Scene::get_mask_from_entities_at(NomadIndex layer, const CircleF& circle, const Entity* exclude) const {
//    if (layer >= m_layers.size()) {
//        return 0;
//    }
//
//    NomadInteger mask = 0;
//
//    for (auto entity : m_layers[layer].entities) {
//        if (entity == exclude) {
//            continue;
//        }
//
//        if (entity->is_touching(circle)) {
//            mask |= entity->get_mask();
//        }
//    }
//
//    return mask;
    return 0;
}

NomadInteger Scene::get_mask_in_rectangle(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    NomadInteger mask = 0;

    mask |= get_mask_from_entities_at(layer, rectangle, exclude);

    Rectangle tile_area {
        static_cast<NomadInteger>(rectangle.get_left()),
        static_cast<NomadInteger>(rectangle.get_top()),
        static_cast<NomadInteger>(rectangle.get_right()),
        static_cast<NomadInteger>(rectangle.get_bottom())
    };

    process_tile_at(layer, tile_area, [&mask](const TileInformation& information) {
        mask |= information.mask;
    });

    return mask;
}

NomadInteger Scene::get_mask_in_circle(NomadIndex layer, const CircleF& circle, const Entity* exclude) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    NomadInteger mask = 0;

    mask |= get_mask_from_entities_at(layer, circle, exclude);

    Rectangle tile_area {
        static_cast<NomadInteger>(circle.get_x() - circle.get_radius()),
        static_cast<NomadInteger>(circle.get_y() - circle.get_radius()),
        static_cast<NomadInteger>(circle.get_x() + circle.get_radius()),
        static_cast<NomadInteger>(circle.get_y() + circle.get_radius())
    };

    process_tile_at(layer, tile_area, [&mask](const TileInformation& information) {
        mask |= information.mask;
    });

    return mask;
}

void Scene::for_each_entities(const std::function<void(Entity*)>& callback) const {
    for (auto entity : m_entities) {
        callback(entity);
    }
}

void Scene::for_each_entity_by_layer(NomadIndex layer_index, const std::function<void(Entity*)>& callback) const {
    if (layer_index >= m_layers.size()) {
        return;
    }

    auto& layer = m_layers[layer_index];

    for (auto entity : layer.entities) {
        callback(entity);
    }
}

} // nomad