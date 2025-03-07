//
// Created by jfbil on 2024-11-22.
//

#include "nomad/game/Scene.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Text.hpp"

#include "nomad/script/Runtime.hpp"

#include <algorithm>

namespace nomad {

// Scene tick update
void Scene::update(Game* game) {
    const auto script_id = m_events.get_script_id_for_event("update");

    if (script_id != NOMAD_INVALID_ID) {
        game->execute_script_in_context(script_id, &m_execution_context);
    }

    for (const auto entity : m_entities) {
        entity->update(this);
    }

    // Remove entities
    for (const auto entity : m_removed_entities) {
        const auto it = std::find(m_entities.begin(), m_entities.end(), entity);

        if (it != m_entities.end()) {
            m_entities.erase(it);
        }
    }
    m_removed_entities.clear();

    // Update layers physics
    update_physics();

    // Update camera
    update_camera();

    // Process added and removed entities
    process_add_remove_entities();

    // Clear 'pressed' and 'released' flags on action mapping
    for (auto& map : m_action_mapping) {
        map.pressed = false;
        map.released = false;
    }
}

void Scene::update_physics() {
    auto time_step = 1.0f / static_cast<float>(m_game->get_fps());

    for (auto& layer : m_layers) {
        // Check for invalidated walls.
        if (layer.walls_invalidated) {
            layer.walls_invalidated = false;

            // Destroy existing walls.
            for (auto& wall : layer.walls) {
                b2DestroyBody(wall);
            }

            // Re-create walls
            auto tile_half_width = static_cast<float>(m_tile_width) / 2.0f;
            auto tile_half_height = static_cast<float>(m_tile_height) / 2.0f;

            for (auto y = 0; y < m_tile_map_height; ++y) {
                for (auto x = 0; x < m_tile_map_width; ++x) {
                    auto tile_index = layer.wall_tile_map[y * m_tile_map_width + x];

                    if (tile_index == 0) {
                        continue;
                    }

                    auto x1 = x * m_tile_width;
                    auto y1 = y * m_tile_height;
                    auto x2 = x1 + m_tile_width;
                    auto y2 = y1 + m_tile_height;

                    auto body_def = b2DefaultBodyDef();

                    body_def.type = b2_staticBody;
                    body_def.position = b2Vec2{
                        static_cast<float>(x1) + tile_half_width,
                        static_cast<float>(y1) + tile_half_height
                    };

                    auto body_id = b2CreateBody(layer.world_id, &body_def);

                    b2Polygon rectangle = b2MakeBox(
                        tile_half_width,
                        tile_half_width
                    );

                    b2ShapeDef shape_def = b2DefaultShapeDef();
                    shape_def.filter = m_wall_filter;
                    b2CreatePolygonShape(body_id, &shape_def, &rectangle);

                    layer.walls.push_back(body_id);
                }
            }
        }

        auto world_id = layer.world_id;

        for (auto entity : layer.entities) {
            entity->before_simulation_update(world_id);
        }

        b2World_Step(world_id, time_step, 4);

        auto sensor_events = b2World_GetSensorEvents(world_id);

        for (auto i = 0; i < sensor_events.beginCount; ++i) {
            auto sensor_event = sensor_events.beginEvents[i];

            auto shape_id_sensor = sensor_event.sensorShapeId;
            auto shape_id_visitor = sensor_event.visitorShapeId;
            auto body_id_sensor = b2Shape_GetBody(shape_id_sensor);
            auto body_id_visitor = b2Shape_GetBody(shape_id_visitor);

            auto entity_sensor = static_cast<Entity*>(b2Body_GetUserData(body_id_sensor));
            auto entity_visitor = static_cast<Entity*>(b2Body_GetUserData(body_id_visitor));

            auto script_id_sensor = entity_sensor ? entity_sensor->get_on_collision_begin() : NOMAD_INVALID_ID;

            if (script_id_sensor != NOMAD_INVALID_ID) {
                if (entity_visitor) {
                    m_game->execute_script_in_new_context(script_id_sensor, this, entity_sensor, entity_visitor);
                } else {
                    m_game->execute_script_in_new_context(script_id_sensor, this, entity_sensor);
                }
            }
        }

        for (auto i = 0; i < sensor_events.endCount; ++i) {
            auto sensor_event = sensor_events.endEvents[i];

            auto shape_id_sensor = sensor_event.sensorShapeId;
            auto shape_id_visitor = sensor_event.visitorShapeId;
            auto body_id_sensor = b2Shape_GetBody(shape_id_sensor);
            auto body_id_visitor = b2Shape_GetBody(shape_id_visitor);

            auto entity_sensor = static_cast<Entity*>(b2Body_GetUserData(body_id_sensor));
            auto entity_visitor = static_cast<Entity*>(b2Body_GetUserData(body_id_visitor));

            auto script_id_sensor = entity_sensor ? entity_sensor->get_on_collision_end() : NOMAD_INVALID_ID;

            if (script_id_sensor != NOMAD_INVALID_ID) {
                if (entity_visitor) {
                    m_game->execute_script_in_new_context(script_id_sensor, this, entity_sensor, entity_visitor);
                } else {
                    m_game->execute_script_in_new_context(script_id_sensor, this, entity_sensor);
                }
            }
        }

        // Process collisions
        auto contact_events = b2World_GetContactEvents(world_id);

        for (auto i = 0; i < contact_events.beginCount; ++i) {
            auto contact_event = contact_events.beginEvents[i];

            auto shape_id_a = contact_event.shapeIdA;
            auto shape_id_b = contact_event.shapeIdB;
            auto body_id_a = b2Shape_GetBody(shape_id_a);
            auto body_id_b = b2Shape_GetBody(shape_id_b);

            auto entity_a = static_cast<Entity*>(b2Body_GetUserData(body_id_a));
            auto entity_b = static_cast<Entity*>(b2Body_GetUserData(body_id_b));

            auto script_id_a = entity_a ? entity_a->get_on_collision_begin() : NOMAD_INVALID_ID;
            auto script_id_b = entity_b ? entity_b->get_on_collision_begin() : NOMAD_INVALID_ID;

            if (script_id_a != NOMAD_INVALID_ID && entity_b) {
                m_game->execute_script_in_new_context(script_id_a, this, entity_a, entity_b);
            }

            if (script_id_b != NOMAD_INVALID_ID && entity_a) {
                m_game->execute_script_in_new_context(script_id_b, this, entity_b, entity_a);
            }
        }

        for (auto i = 0; i < contact_events.endCount; ++i) {
            log::info("begin");
            auto contact_event = contact_events.endEvents[i];

            auto shape_id_a = contact_event.shapeIdA;
            auto shape_id_b = contact_event.shapeIdB;
            auto body_id_a = b2Shape_GetBody(shape_id_a);
            auto body_id_b = b2Shape_GetBody(shape_id_b);

            auto entity_a = static_cast<Entity*>(b2Body_GetUserData(body_id_a));
            auto entity_b = static_cast<Entity*>(b2Body_GetUserData(body_id_b));

            auto script_id_a = entity_a ? entity_a->get_on_collision_end() : NOMAD_INVALID_ID;
            auto script_id_b = entity_b ? entity_b->get_on_collision_end() : NOMAD_INVALID_ID;

            if (script_id_a != NOMAD_INVALID_ID) {
                if (entity_b) {
                    m_game->execute_script_in_new_context(script_id_a, this, entity_a, entity_b);
                } else {
                    m_game->execute_script_in_new_context(script_id_a, this, entity_a);
                }
            }

            if (script_id_b != NOMAD_INVALID_ID) {
                if (entity_a) {
                    m_game->execute_script_in_new_context(script_id_b, this, entity_b, entity_a);
                } else {
                    m_game->execute_script_in_new_context(script_id_b, this, entity_b);
                }
            }
        }

        for (auto entity : layer.entities) {
            entity->after_simulation_update(world_id);
        }
    }
}

void Scene::update_entity_layers() {
    for (NomadIndex layer_index = 0; layer_index < m_layers.size(); ++layer_index) {
        auto layer = &m_layers[layer_index];

        // If entity is not in the right layer, move it to its layer.
        NomadIndex entity_index = 0;

        while (entity_index < layer->entities.size()) {
            auto entity = layer->entities[entity_index];
            auto entity_layer = entity->get_layer();
            if (entity_layer != layer_index) {
                if (entity_layer < 0 || entity_layer >= m_layers.size()) {
                    log::warning("Entity '" + entity->get_name() + "' has an invalid layer: " + std::to_string(entity_layer));
                    log::warning("Defaulting to layer 0");

                    entity_layer = 0;

                    entity->set_layer(entity_layer);

                    if (layer_index == 0) {
                        // Already in the right layer
                        continue;
                    }
                }

                m_layers[entity_layer].entities.push_back(entity);
                layer->entities.erase(layer->entities.begin() + entity_index);

                // Do not advance index so we do not skip over the next entity.
                continue;
            }
            else {
                // Entity is in the right layer, move to next entity.
                ++entity_index;
            }
        }
    }
}

void Scene::update_camera() {
    if (m_camera_follow_entity_id != NOMAD_INVALID_ID) {
        auto entity = get_entity_by_id(m_camera_follow_entity_id);

        if (entity) {
            m_camera_position.set(entity->get_location());
        } else {
            // Entity no longer exists, stop following
            m_camera_follow_entity_id = NOMAD_INVALID_ID;
        }
    }

    // Update which entities are in the camera view
    auto resolution = m_game->get_resolution().to_pointf();

    auto camera_rectangle = RectangleF{
        m_camera_position.x() - resolution.x() / 2,
        m_camera_position.y() - resolution.y() / 2,
        resolution.x(),
        resolution.y()
    };

    for (auto& entity : m_entities) {
        RectangleF bounding_box;

        entity->get_bounding_box(bounding_box);

        if (camera_rectangle.intersects(bounding_box)) {
            entity->enter_camera();
        } else {
            entity->exit_camera();
        }
    }
}

void Scene::process_add_remove_entities() {
    // Add entities
    for (const auto& added_entity : m_added_entities) {
        auto entity_id = added_entity.id;

        if (entity_id == NOMAD_INVALID_ID) {
            entity_id = get_next_entity_id();

            if (entity_id == NOMAD_INVALID_ID) {
                // Hopefully, should never happen
                log::error("Could not create entity: no more entity IDs available");
                continue; // skip!
            }
        }

        const auto entity = new Entity(this, m_game->get_entity_variables(), entity_id, added_entity.x, added_entity.y, added_entity.layer);

        if (!added_entity.text_id.empty()) {
            auto text = m_game->get_text(added_entity.text_id);
            entity->set_text(text);
        }

        ScriptValue return_value;

        auto script = m_game->get_runtime()->get_script(added_entity.init_script_id);

        entity->set_name(script->get_name()); // Set default name to the script.
        entity->set_script_name(script->get_name());

        m_game->execute_script_in_new_context(added_entity.init_script_id, this, entity, return_value);

        m_entities.push_back(entity);

        // Add entity to layer
        auto entity_layer = entity->get_layer();

        if (entity_layer < 0 || entity_layer >= m_layers.size()) {
            log::warning("Entity '" + entity->get_name() + "' has an invalid layer id: " + std::to_string(entity_layer) + ". Defaulting to layer 0");

            entity_layer = 0;
        }

        entity->set_layer(entity_layer);

        m_layers[entity_layer].entities.push_back(entity);
    }

    m_added_entities.clear();

    // Make sure entities are in the right layer
    update_entity_layers();

    // Sort entities by 'z'
    for (auto& layer : m_layers) {
        std::sort(layer.entities.begin(), layer.entities.end(), [](auto a, auto b) {
            return a->get_z() < b->get_z();
        });
    }
}

} // nomad