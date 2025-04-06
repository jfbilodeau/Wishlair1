//
// Created by jfbil on 2024-11-22.
//

#include "nomad/game/Scene.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Runtime.hpp"

#include <algorithm>

namespace nomad {

// Scene tick update
void Scene::update(Game* game) {
    const auto script_id = m_events.getScriptIdForEvent("update");

    if (script_id != NOMAD_INVALID_ID) {
        game->executeScriptInContext(script_id, &m_executionContext);
    }

    for (const auto entity : m_entities) {
        if (!entity->isPaused()) {
            entity->update(this);
        }
    }

    // Remove entities
    processRemoveEntities();

    // Update layers physics
    updatePhysics();

    // Update camera
    updateCamera();

    // Process added entities
    processAddEntities();

    // Update scheduled events
    updateScheduledEvents();

    // Make sure entities are in the right layer
    updateEntityLayers();

    // Sort entities by 'z'
    for (auto& layer : m_layers) {
        std::sort(layer.entities.begin(), layer.entities.end(), [](auto a, auto b) {
            return a->getZ() < b->getZ();
        });
    }

    // Clear 'pressed' and 'released' flags on action mapping
    for (auto& map : m_actionMapping) {
        map.pressed = false;
        map.released = false;
    }

    // Increment frame number
    ++m_frameNumber;
}

void Scene::updateScheduledEvents() {
    auto current_frame = getFrameNumber();
    auto it = m_scheduledEvents.begin();

    while (it != m_scheduledEvents.end() && it->frameNumber <= current_frame) {
        triggerEvent(it->name);

        it = m_scheduledEvents.erase(it);
    }
}

void Scene::updatePhysics() {
    auto time_step = 1.0f / static_cast<float>(m_game->getFps());

    for (auto& layer : m_layers) {
        // Check for invalidated walls.
        if (layer.wallsInvalidated) {
            layer.wallsInvalidated = false;

            // Destroy existing walls.
            for (auto& wall : layer.walls) {
                b2DestroyBody(wall);
            }

            // Re-create walls
            auto tile_half_width = static_cast<float>(m_tileWidth) / 2.0f;
            auto tile_half_height = static_cast<float>(m_tileHeight) / 2.0f;

            for (auto y = 0; y < m_tileMapHeight; ++y) {
                for (auto x = 0; x < m_tileMapWidth; ++x) {
                    auto tile_index = layer.wallTileMap[y * m_tileMapWidth + x];

                    if (tile_index == 0) {
                        continue;
                    }

                    auto x1 = x * m_tileWidth;
                    auto y1 = y * m_tileHeight;
                    auto x2 = x1 + m_tileWidth;
                    auto y2 = y1 + m_tileHeight;

                    auto body_def = b2DefaultBodyDef();

                    body_def.type = b2_staticBody;
                    body_def.position = b2Vec2{
                        static_cast<float>(x1) + tile_half_width,
                        static_cast<float>(y1) + tile_half_height
                    };

                    auto body_id = b2CreateBody(layer.worldId, &body_def);

                    b2Polygon rectangle = b2MakeBox(
                        tile_half_width,
                        tile_half_width
                    );

                    b2ShapeDef shape_def = b2DefaultShapeDef();
                    shape_def.filter = m_wallFilter;
                    b2CreatePolygonShape(body_id, &shape_def, &rectangle);

                    layer.walls.push_back(body_id);
                }
            }
        }

        auto world_id = layer.worldId;

        for (auto entity : layer.entities) {
            entity->beforeSimulationUpdate(world_id);
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

            auto script_id_sensor = entity_sensor ? entity_sensor->getOnCollisionBegin() : NOMAD_INVALID_ID;

            if (script_id_sensor != NOMAD_INVALID_ID) {
                if (entity_visitor) {
                    m_game->executeScriptInNewContext(script_id_sensor, this, entity_sensor, entity_visitor);
                } else {
                    m_game->executeScriptInNewContext(script_id_sensor, this, entity_sensor);
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

            auto script_id_sensor = entity_sensor ? entity_sensor->getOnCollisionEnd() : NOMAD_INVALID_ID;

            if (script_id_sensor != NOMAD_INVALID_ID) {
                if (entity_visitor) {
                    m_game->executeScriptInNewContext(script_id_sensor, this, entity_sensor, entity_visitor);
                } else {
                    m_game->executeScriptInNewContext(script_id_sensor, this, entity_sensor);
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

            auto script_id_a = entity_a ? entity_a->getOnCollisionBegin() : NOMAD_INVALID_ID;
            auto script_id_b = entity_b ? entity_b->getOnCollisionBegin() : NOMAD_INVALID_ID;

            if (script_id_a != NOMAD_INVALID_ID && entity_b) {
                m_game->executeScriptInNewContext(script_id_a, this, entity_a, entity_b);
            }

            if (script_id_b != NOMAD_INVALID_ID && entity_a) {
                m_game->executeScriptInNewContext(script_id_b, this, entity_b, entity_a);
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

            auto script_id_a = entity_a ? entity_a->getOnCollisionEnd() : NOMAD_INVALID_ID;
            auto script_id_b = entity_b ? entity_b->getOnCollisionEnd() : NOMAD_INVALID_ID;

            if (script_id_a != NOMAD_INVALID_ID) {
                if (entity_b) {
                    m_game->executeScriptInNewContext(script_id_a, this, entity_a, entity_b);
                } else {
                    m_game->executeScriptInNewContext(script_id_a, this, entity_a);
                }
            }

            if (script_id_b != NOMAD_INVALID_ID) {
                if (entity_a) {
                    m_game->executeScriptInNewContext(script_id_b, this, entity_b, entity_a);
                } else {
                    m_game->executeScriptInNewContext(script_id_b, this, entity_b);
                }
            }
        }

        for (auto entity : layer.entities) {
            entity->afterSimulationUpdate(world_id);
        }
    }
}

void Scene::updateEntityLayers() {
    for (NomadIndex layer_index = 0; layer_index < m_layers.size(); ++layer_index) {
        auto layer = &m_layers[layer_index];

        // If entity is not in the right layer, move it to its layer.
        NomadIndex entity_index = 0;

        while (entity_index < layer->entities.size()) {
            auto entity = layer->entities[entity_index];
            auto entity_layer = entity->getLayer();
            if (entity_layer != layer_index) {
                if (entity_layer < 0 || entity_layer >= m_layers.size()) {
                    log::warning("Entity '" + entity->getName() + "' has an invalid layer: " + std::to_string(entity_layer));
                    log::warning("Defaulting to layer 0");

                    entity_layer = 0;

                    entity->setLayer(entity_layer);

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

void Scene::updateCamera() {
    if (m_cameraFollowEntityId != NOMAD_INVALID_ID) {
        auto entity = getEntityById(m_cameraFollowEntityId);

        if (entity) {
            m_cameraPosition.set(entity->getLocation());
        } else {
            // Entity no longer exists, stop following
            m_cameraFollowEntityId = NOMAD_INVALID_ID;
        }
    }

    // Update which entities are in the camera view
    auto resolution = m_game->getResolution().toPointf();

    auto camera_rectangle = RectangleF{
        m_cameraPosition.getX() - resolution.getX() / 2,
        m_cameraPosition.getY() - resolution.getY() / 2,
        resolution.getX(),
        resolution.getY()
    };

    for (auto& entity : m_entities) {
        RectangleF bounding_box;

        entity->getBoundingBox(bounding_box);

        if (camera_rectangle.intersects(bounding_box)) {
            entity->enterCamera();
        } else {
            entity->exitCamera();
        }
    }
}

void Scene::processRemoveEntities() {
    if (!m_removedEntities.empty()) {
        // Presently, there are no scripts executed when removing entities.
        // But to play it safe, let's copy the list of removed entities to a temporary list
        auto temp_removed_entities = createTempVector(m_removedEntities);

        m_removedEntities.clear();

        for (const auto entity : temp_removed_entities) {
            const auto it = std::ranges::find(m_entities, entity);

            if (it != m_entities.end()) {
                m_entities.erase(it);
            }
        }
    }
}

void Scene::processAddEntities() {
    if (!m_addedEntities.empty()) {
        // While entities are added, their `init` script will run. Some entities
        // may create new entities which invalidates the `added_entities` list.
        // To address this, let's create a new, temporary list of added entities
        auto temp_added_entities = createTempVector(m_addedEntities);

        m_addedEntities.clear();

        // Add entities
        for (const auto& added_entity : temp_added_entities) {
            auto entity_id = added_entity.id;

            if (entity_id == NOMAD_INVALID_ID) {
                entity_id = getNextEntityId();

                if (entity_id == NOMAD_INVALID_ID) {
                    // Hopefully, should never happen
                    log::error("Could not create entity: no more entity IDs available");
                    continue; // skip!
                }
            }

            const auto entity = new Entity(this, m_game->getEntityVariables(), entity_id, added_entity.x, added_entity.y, added_entity.layer);

            if (!added_entity.textId.empty()) {
                NomadString text;
                m_game->getText(added_entity.textId, text);
                entity->setText(text);
            }

            ScriptValue return_value;

            auto script = m_game->getRuntime()->getScript(added_entity.initScriptId);

            entity->setName(script->getName()); // Set default name to the script.
            entity->setScriptName(script->getName());

            m_game->executeScriptInNewContext(added_entity.initScriptId, this, entity, return_value);

            m_entities.push_back(entity);

            // Add entity to layer
            auto entity_layer = entity->getLayer();

            if (entity_layer < 0 || entity_layer >= m_layers.size()) {
                log::warning("Entity '" + entity->getName() + "' has an invalid layer id: " + std::to_string(entity_layer) + ". Defaulting to layer 0");

                entity_layer = 0;
            }

            entity->setLayer(entity_layer);

            m_layers[entity_layer].entities.push_back(entity);
        }
    }
}

} // nomad