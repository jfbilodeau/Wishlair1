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
    const auto scriptId = m_events.getScriptIdForEvent("update");

    if (scriptId != NOMAD_INVALID_ID) {
        game->executeScriptInContext(scriptId, &m_executionContext);
    }

    for (const auto entity : m_entities) {
        if (!entity->isPaused()) {
            entity->update(this);
        }
    }

    // Remove entities
    processRemoveEntities();

    // Update layers physics
    if (m_physicsEnabled) {
        updatePhysics();
    }

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
        std::ranges::sort(layer.entities, [](auto a, auto b) {
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
    auto currentFrame = getFrameNumber();
    auto it = m_scheduledEvents.begin();

    while (it != m_scheduledEvents.end() && it->frameNumber <= currentFrame) {
        triggerEvent(it->name);

        it = m_scheduledEvents.erase(it);
    }
}

void Scene::updatePhysics() {
    auto timeStep = 1.0f / static_cast<float>(m_game->getFps());

    for (auto& layer : m_layers) {
        // Check for invalidated walls.
        if (layer.wallsInvalidated) {
            layer.wallsInvalidated = false;

            // Destroy existing walls.
            for (auto& wall : layer.walls) {
                b2DestroyBody(wall);
            }

            // Re-create walls
            auto tileHalfWidth = static_cast<float>(m_tileWidth) / 2.0f;
            auto tileHalfHeight = static_cast<float>(m_tileHeight) / 2.0f;

            for (auto y = 0; y < m_tileMapHeight; ++y) {
                for (auto x = 0; x < m_tileMapWidth; ++x) {
                    auto tileIndex = layer.wallTileMap[y * m_tileMapWidth + x];

                    if (tileIndex == 0) {
                        continue;
                    }

                    auto x1 = x * m_tileWidth;
                    auto y1 = y * m_tileHeight;
                    auto x2 = x1 + m_tileWidth;
                    auto y2 = y1 + m_tileHeight;

                    auto bodyDef = b2DefaultBodyDef();

                    bodyDef.type = b2_staticBody;
                    bodyDef.position = b2Vec2{
                        static_cast<float>(x1) + tileHalfWidth,
                        static_cast<float>(y1) + tileHalfHeight
                    };

                    auto bodyId = b2CreateBody(layer.worldId, &bodyDef);

                    b2Polygon rectangle = b2MakeBox(
                        tileHalfWidth,
                        tileHalfWidth
                    );

                    b2ShapeDef shapeDef = b2DefaultShapeDef();
                    shapeDef.filter = m_wallFilter;
                    b2CreatePolygonShape(bodyId, &shapeDef, &rectangle);

                    layer.walls.push_back(bodyId);
                }
            }
        }

        auto worldId = layer.worldId;

        for (auto entity : layer.entities) {
            entity->beforeSimulationUpdate(worldId);
        }

        b2World_Step(worldId, timeStep, 4);

        auto sensorEvents = b2World_GetSensorEvents(worldId);

        for (auto i = 0; i < sensorEvents.beginCount; ++i) {
            auto sensorEvent = sensorEvents.beginEvents[i];

            auto shapeIdSensor = sensorEvent.sensorShapeId;
            auto shapeIdVisitor = sensorEvent.visitorShapeId;
            auto bodyIdSensor = b2Shape_GetBody(shapeIdSensor);
            auto bodyIdVisitor = b2Shape_GetBody(shapeIdVisitor);

            auto entitySensor = static_cast<Entity*>(b2Body_GetUserData(bodyIdSensor));
            auto entityVisitor = static_cast<Entity*>(b2Body_GetUserData(bodyIdVisitor));

            auto scriptIdSensor = entitySensor ? entitySensor->getOnCollisionBegin() : NOMAD_INVALID_ID;

            if (scriptIdSensor != NOMAD_INVALID_ID) {
                if (entityVisitor) {
                    m_game->executeScriptInNewContext(scriptIdSensor, this, entitySensor, entityVisitor);
                } else {
                    m_game->executeScriptInNewContext(scriptIdSensor, this, entitySensor);
                }
            }
        }

        for (auto i = 0; i < sensorEvents.endCount; ++i) {
            auto sensorEvent = sensorEvents.endEvents[i];

            auto shapeIdSensor = sensorEvent.sensorShapeId;
            auto shapeIdVisitor = sensorEvent.visitorShapeId;
            auto bodyIdSensor = b2Shape_GetBody(shapeIdSensor);
            auto bodyIdVisitor = b2Shape_GetBody(shapeIdVisitor);

            auto entitySensor = static_cast<Entity*>(b2Body_GetUserData(bodyIdSensor));
            auto entityVisitor = static_cast<Entity*>(b2Body_GetUserData(bodyIdVisitor));

            auto scriptIdSensor = entitySensor ? entitySensor->getOnCollisionEnd() : NOMAD_INVALID_ID;

            if (scriptIdSensor != NOMAD_INVALID_ID) {
                if (entityVisitor) {
                    m_game->executeScriptInNewContext(scriptIdSensor, this, entitySensor, entityVisitor);
                } else {
                    m_game->executeScriptInNewContext(scriptIdSensor, this, entitySensor);
                }
            }
        }

        // Process collisions
        auto contactEvents = b2World_GetContactEvents(worldId);

        for (auto i = 0; i < contactEvents.beginCount; ++i) {
            auto contactEvent = contactEvents.beginEvents[i];

            auto shapeIdA = contactEvent.shapeIdA;
            auto shapeIdB = contactEvent.shapeIdB;
            auto bodyIdA = b2Shape_GetBody(shapeIdA);
            auto bodyIdB = b2Shape_GetBody(shapeIdB);

            auto entityA = static_cast<Entity*>(b2Body_GetUserData(bodyIdA));
            auto entityB = static_cast<Entity*>(b2Body_GetUserData(bodyIdB));

            auto scriptIdA = entityA ? entityA->getOnCollisionBegin() : NOMAD_INVALID_ID;
            auto scriptIdB = entityB ? entityB->getOnCollisionBegin() : NOMAD_INVALID_ID;

            if (scriptIdA != NOMAD_INVALID_ID && entityB) {
                m_game->executeScriptInNewContext(scriptIdA, this, entityA, entityB);
            }

            if (scriptIdB != NOMAD_INVALID_ID && entityA) {
                m_game->executeScriptInNewContext(scriptIdB, this, entityB, entityA);
            }
        }

        for (auto i = 0; i < contactEvents.endCount; ++i) {
            log::info("begin");
            auto contactEvent = contactEvents.endEvents[i];

            auto shapeIdA = contactEvent.shapeIdA;
            auto shapeIdB = contactEvent.shapeIdB;
            auto bodyIdA = b2Shape_GetBody(shapeIdA);
            auto bodyIdB = b2Shape_GetBody(shapeIdB);

            auto entityA = static_cast<Entity*>(b2Body_GetUserData(bodyIdA));
            auto entityB = static_cast<Entity*>(b2Body_GetUserData(bodyIdB));

            auto scriptIdA = entityA ? entityA->getOnCollisionEnd() : NOMAD_INVALID_ID;
            auto scriptIdB = entityB ? entityB->getOnCollisionEnd() : NOMAD_INVALID_ID;

            if (scriptIdA != NOMAD_INVALID_ID) {
                if (entityB) {
                    m_game->executeScriptInNewContext(scriptIdA, this, entityA, entityB);
                } else {
                    m_game->executeScriptInNewContext(scriptIdA, this, entityA);
                }
            }

            if (scriptIdB != NOMAD_INVALID_ID) {
                if (entityA) {
                    m_game->executeScriptInNewContext(scriptIdB, this, entityB, entityA);
                } else {
                    m_game->executeScriptInNewContext(scriptIdB, this, entityB);
                }
            }
        }

        for (auto entity : layer.entities) {
            entity->afterSimulationUpdate(worldId);
        }
    }
}

void Scene::updateEntityLayers() {
    for (NomadIndex layerIndex = 0; layerIndex < m_layers.size(); ++layerIndex) {
        auto layer = &m_layers[layerIndex];

        // If entity is not in the right layer, move it to its layer.
        NomadIndex entityIndex = 0;

        while (entityIndex < layer->entities.size()) {
            auto entity = layer->entities[entityIndex];
            auto entityLayer = entity->getLayer();
            if (entityLayer != layerIndex) {
                if (entityLayer < 0 || entityLayer >= m_layers.size()) {
                    log::warning("Entity '" + entity->getName() + "' has an invalid layer: " + std::to_string(entityLayer));
                    log::warning("Defaulting to layer 0");

                    entityLayer = 0;

                    entity->setLayer(entityLayer);

                    if (layerIndex == 0) {
                        // Already in the right layer
                        continue;
                    }
                }

                m_layers[entityLayer].entities.push_back(entity);
                layer->entities.erase(layer->entities.begin() + entityIndex);

                // Do not advance index so we do not skip over the next entity.
                continue;
            }
            else {
                // Entity is in the right layer, move to next entity.
                ++entityIndex;
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

    auto cameraRectangle = RectangleF{
        m_cameraPosition.getX() - resolution.getX() / 2,
        m_cameraPosition.getY() - resolution.getY() / 2,
        resolution.getX(),
        resolution.getY()
    };

    for (auto& entity : m_entities) {
        RectangleF boundingBox;

        entity->getBoundingBox(boundingBox);

        if (cameraRectangle.intersects(boundingBox)) {
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
        auto tempRemovedEntities = createTempVector(m_removedEntities);

        m_removedEntities.clear();

        for (const auto entity : tempRemovedEntities) {
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
        auto tempAddedEntities = createTempVector(m_addedEntities);

        m_addedEntities.clear();

        // Add entities
        for (const auto& addedEntity : tempAddedEntities) {
            auto entityId = addedEntity.id;

            if (entityId == NOMAD_INVALID_ID) {
                entityId = getNextEntityId();

                if (entityId == NOMAD_INVALID_ID) {
                    // Hopefully, should never happen
                    log::error("Could not create entity: no more entity IDs available");
                    continue; // skip!
                }
            }

            const auto entity = new Entity(this, m_game->getEntityVariables(), entityId, addedEntity.x, addedEntity.y, addedEntity.layer);

            if (!addedEntity.textId.empty()) {
                NomadString text;
                m_game->getText(addedEntity.textId, text);
                entity->setText(text);
            }

            ScriptValue returnValue;

            auto script = m_game->getRuntime()->getScript(addedEntity.initScriptId);

            entity->setName(script->getName()); // Set default name to the script.
            entity->setScriptName(script->getName());

            m_game->executeScriptInNewContext(addedEntity.initScriptId, this, entity, returnValue);

            m_entities.push_back(entity);

            // Add entity to layer
            auto entityLayer = entity->getLayer();

            if (entityLayer < 0 || entityLayer >= m_layers.size()) {
                log::warning("Entity '" + entity->getName() + "' has an invalid layer id: " + std::to_string(entityLayer) + ". Defaulting to layer 0");

                entityLayer = 0;
            }

            entity->setLayer(entityLayer);

            m_layers[entityLayer].entities.push_back(entity);
        }
    }
}

} // nomad