//
// Created by jfbilodeau on 23-06-14.
//

#include "nomad/game/Scene.hpp"

#include "nomad/geometry/Rectangle.hpp"
#include "nomad/geometry/CircleF.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Texture.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/script/Runtime.hpp"

#include <algorithm>

namespace nomad {

Scene::Scene(Game* game):
    m_game(game),
    m_variables(game->getSceneVariables())
{
    // Create Box2D worlds
    for (NomadId id = 0; id < SCENE_LAYER_COUNT; ++id) {
        auto& layer = m_layers[id];

        layer.id = id;

        b2WorldDef world_def = b2DefaultWorldDef();

        // No gravity
        world_def.gravity = { 0.0f, 0.0f };
        // world_def.hitEventThreshold = 0.001f;
        // world_def.enableSleep = false;
        // world_def.contactHertz = 30.0f;

        layer.worldId = b2CreateWorld(&world_def);
    }
}

Scene::~Scene() {
    for (auto& layer : m_layers) {
        b2DestroyWorld(layer.worldId);
    }
}

Game* Scene::getGame() const {
    return m_game;
}

void Scene::setGame(Game* game) {
    this->m_game = game;
}

void Scene::setName(const NomadString& name) {
    m_name = name;
}

NomadString Scene::getName() const {
    return m_name;
}

void Scene::setZ(NomadInteger z) {
    this->m_z = z;
}

NomadInteger Scene::getZ() const {
    return m_z;
}

void Scene::setFrameNumber(NomadInteger frameNumber) {
    m_frameNumber = frameNumber;
}

NomadInteger Scene::getFrameNumber() const {
    return m_frameNumber;
}

void Scene::processInputEvent(const InputEvent& event) {
    NomadString action_name;

    if (m_actionManager.getActionNameForInput(event.code, action_name)) {
        for (auto& map: m_actionMapping) {
            if (
                map.name == action_name &&
                map.action == event.action
            ) {
                map.pressed = event.action == InputAction::Pressed;
                map.released = event.action == InputAction::Released;
                map.held = map.pressed;

                const auto entity = getEntityById(map.entityId);

                m_game->executeScriptInNewContext(map.scriptId, this, entity);
            }
        }
    }
}

void Scene::setVariableValue(NomadId variableId, const ScriptValue& value) {
    m_variables.setVariableValue(variableId, value);
}

void Scene::getVariableValue(NomadId variableId, ScriptValue& value) const {
    m_variables.getVariableValue(variableId, value);
}

void Scene::createEntity(
    const NomadString& initScriptName,
    NomadFloat x,
    NomadFloat y,
    NomadInteger layer,
    NomadId id,
    const NomadString& text
) {
    const auto init_script_id = m_game->getScriptId(initScriptName);

    if (init_script_id == NOMAD_INVALID_ID) {
        log::error("Script for entity '" + initScriptName + "' not found");
        return; // skip!;
    }

    m_addedEntities.push_back({
        init_script_id,
        x,
        y,
        layer,
        id,
        text
    });
}

void Scene::removeEntity(Entity* entity) {
    // Make sure entity is not already in the list of removed entities
    const auto it = std::find(m_removedEntities.begin(), m_removedEntities.end(), entity);

    if (it == m_removedEntities.end()) {
        m_removedEntities.push_back(entity);
    }
}

Entity* Scene::getEntityById(NomadId id) const {
    for (const auto entity : m_entities) {
        if (entity->getId() == id) {
            return entity;
        }
    }

    return nullptr;
}

Entity* Scene::getEntityByName(const NomadString& name) const {
    for (const auto entity : m_entities) {
        if (entity->getName() == name) {
            return entity;
        }
    }

    return nullptr;
}

void Scene::getEntitiesByName(const NomadString& name, EntityList& entities) const {
    for (const auto entity : m_entities) {
        if (entity->getName() == name) {
            entities.push_back(entity);
        }
    }
}

void Scene::pauseOtherEntities(Entity* entity) {
    for (auto other_entity : m_entities) {
        if (other_entity != entity) {
            other_entity->pause();
        }
    }
}

void Scene::pauseOtherEntities(const std::vector<Entity*>& entities) {
    for (auto other_entity : m_entities) {
        if (std::find(entities.begin(), entities.end(), other_entity) == entities.end()) {
            other_entity->pause();
        }
    }
}

void Scene::pauseAllEntities() {
    for (const auto entity : m_entities) {
        entity->pause();
    }
}

void Scene::unpauseAllEntities() {
    for (const auto entity : m_entities) {
        entity->unpause();
    }
}

void Scene::unpauseAllVisibleEntities() {
    for (const auto entity : m_entities) {
        if (entity->isVisible()) {
            entity->unpause();
        }
    }
}

void Scene::addEvent(const NomadString& name, NomadId scriptId) {
    m_events.addEvent(name, scriptId);
}

void Scene::removeEvent(const NomadString& name) {
    m_events.removeEvent(name);
}

void Scene::loadActionMapping(const NomadString& mappingName) {
    m_actionManager.loadMapping(m_game, mappingName);
}

void Scene::saveActionMapping(const NomadString& mappingName) {
    m_actionManager.saveMapping(m_game, mappingName);
}

void Scene::resetActionMapping(const NomadString& mappingName) {
    m_actionManager.resetMappingToDefaults(m_game, mappingName);
}

void Scene::addActionPressed(const NomadString& actionName, NomadId scriptId, NomadId entityId) {
    auto mapping = getActionMapping(actionName, InputAction::Pressed, entityId);

    if (mapping) {
        // Update mapping:
        mapping->scriptId = scriptId;
    } else {
        m_actionMapping.emplace_back(
            ActionMapping{
                actionName,
                InputAction::Pressed,
                scriptId,
                entityId
            }
        );
    }
}

void Scene::addActionReleased(const NomadString& actionName, NomadId id, NomadId entityId) {
    auto mapping = getActionMapping(actionName, InputAction::Released, entityId);

    if (mapping) {
        // Update mapping:
        mapping->scriptId = id;
    } else {
        m_actionMapping.emplace_back(
            ActionMapping{
                actionName,
                InputAction::Released,
                id,
                entityId
            }
        );
    }
}

void Scene::removeActionPressed(const NomadString& actionName, NomadId entityId) {
    const auto i = std::find_if(m_actionMapping.begin(), m_actionMapping.end(), [actionName, entityId](const ActionMapping& mapping) {
        return mapping.name == actionName && mapping.action == InputAction::Pressed && mapping.entityId == entityId;
    });

    m_actionMapping.erase(i);
}

void Scene::removeActionReleased(const NomadString& actionName, NomadId entityId) {
    const auto i = std::find_if(m_actionMapping.begin(), m_actionMapping.end(), [actionName, entityId](const ActionMapping& mapping) {
        return mapping.name == actionName && mapping.action == InputAction::Pressed && mapping.entityId == entityId;
    });

    m_actionMapping.erase(i);
}

NomadId Scene::getNextEntityId() {
    const auto start_id = m_entityIdCounter;

    if (m_entities.size() >= NOMAD_ID_MAX - NOMAD_ID_MIN) {
        // We're already at the max number of entities.
        return NOMAD_INVALID_ID;
    }

    while (true) {
        const auto entity = getEntityById(m_entityIdCounter);

        if (entity == nullptr) {
            return m_entityIdCounter;
        }

        m_entityIdCounter++;

        if (m_entityIdCounter == NOMAD_ID_MAX) {
            m_entityIdCounter = NOMAD_ID_MIN;
        }

        if (m_entityIdCounter == start_id) {
            return NOMAD_INVALID_ID;
        }
    }
}

Scene::ActionMapping* Scene::getActionMapping(const NomadString& name, InputAction type, NomadId entityId) {
    for (auto& mapping : m_actionMapping) {
        if (mapping.name == name && mapping.action == type && mapping.entityId == entityId) {
            return &mapping;
        }
    }

    return nullptr;
}

void Scene::setTileSet(const Texture* texture, NomadInteger tileWidth, NomadInteger tileHeight, NomadIndex firstTileIndex) {
    m_tileTexture = texture;
//    m_tile_texture_width = texture->get_width();
//    m_tile_texture_height = texture->get_height();
    auto horizontal_tile_count = texture->getWidth() / tileWidth;
    auto vertical_tile_count = texture->getHeight() / tileHeight;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_tileCount = horizontal_tile_count * vertical_tile_count + firstTileIndex; // +1 to accommodate for tile '0' which is not part of a tileset in Tiled
    m_tiles.resize(m_tileCount);

    // Reset tile masks to 0
    for (auto& tile : m_tiles) {
        tile.mask = 0;
    }

    // Init source coordinates for each tile
    for (NomadIndex y = 0; y < vertical_tile_count; ++y) {
        for (NomadIndex x = 0; x < horizontal_tile_count; ++x) {
            NomadIndex tile_index = y * vertical_tile_count + x + firstTileIndex;
            m_tiles[tile_index].source = Rectangle(
                x * tileWidth,
                y * tileHeight,
                tileWidth,
                tileHeight
            );
        }
    }
}

void Scene::setTileMask(NomadIndex tileIndex, NomadInteger tileMask) {
    if (tileIndex >= m_tiles.size()) {
        return;
    }

    m_tiles[tileIndex].mask = tileMask;
}

NomadInteger Scene::getTileMask(NomadIndex tileIndex) const {
    if (tileIndex >= m_tiles.size()) {
        return 0;
    }

    return m_tiles[tileIndex].mask;
}

void Scene::setTileMapSize(NomadInteger width, NomadInteger height) {
    for (auto& layer : m_layers) {
        std::vector<NomadIndex> new_ground_tile_layer(width * height);
        std::vector<NomadIndex> new_wall_tile_layer(width * height);

        for (NomadIndex y = 0; y < height; ++y) {
            for (NomadIndex x = 0; x < width; ++x) {
                if (x < m_tileMapWidth && y < m_tileMapHeight) {
                    new_ground_tile_layer[y * width + x] = layer.groundTileMap[y * m_tileMapWidth + x];
                    new_wall_tile_layer[y * width + x] = layer.wallTileMap[y * m_tileMapWidth + x];
                } else {
                    new_ground_tile_layer[y * width + x] = 0;
                    new_wall_tile_layer[y * width + x] = 0;
                }
            }
        }

        layer.groundTileMap = std::move(new_ground_tile_layer);
        layer.wallTileMap = std::move(new_wall_tile_layer);
    }

    m_tileMapWidth = width;
    m_tileMapHeight = height;
}

NomadInteger Scene::getTileMapWidth() const {
    return m_tileMapWidth;
}

NomadInteger Scene::getTileMapHeight() const {
    return m_tileMapHeight;
}

void Scene::setGroundTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index) {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    if (x >= m_tileMapWidth || y >= m_tileMapHeight) {
        return;
    }

    tile_layer.groundTileMap[y * m_tileMapWidth + x] = tile_index;
}

void Scene::setWallTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index) {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    if (x >= m_tileMapWidth || y >= m_tileMapHeight) {
        return;
    }

    tile_layer.groundTileMap[y * m_tileMapWidth + x] = tile_index;
}

void Scene::setWallMask(NomadInteger mask) {
    m_wallFilter.categoryBits = mask;

    for (auto& layer : m_layers) {
        for (auto& wall : layer.walls) {
            b2ShapeId shape_id;
            b2Body_GetShapes(wall, &shape_id, 1);
            b2Shape_SetFilter(shape_id, m_wallFilter);
        }
    }
}

NomadInteger Scene::getWallMask() const {
    return m_wallFilter.categoryBits;
}

NomadIndex Scene::getGroundTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (
        layer >= m_layers.size() ||
        x < 0 ||
        x >= m_tileMapWidth ||
        y < 0 ||
        y >= m_tileMapHeight
    ) {
        return 0;
    }

    return m_layers[layer].groundTileMap[y * m_tileMapWidth + x];
}

NomadIndex Scene::getWallTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (
        layer >= m_layers.size() ||
        x < 0 ||
        x >= m_tileMapWidth ||
        y < 0 ||
        y >= m_tileMapHeight
    ) {
        return 0;
    }

    return m_layers[layer].groundTileMap[y * m_tileMapWidth + x];
}

NomadInteger Scene::getTileMask(NomadIndex layer, NomadInteger x, NomadInteger y) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    auto tile_index = getGroundTileIndex(layer, x, y);

    if (tile_index >= m_tiles.size()) {
        return 0;
    }

    return m_tiles[tile_index].mask;
}

void Scene::processTilesAt(NomadIndex layer, const Rectangle& rectangle, TileCallback callback) const {
    if (layer >= m_layers.size()) {
        return;
    }

    auto& tile_layer = m_layers[layer];

    auto tileLeft = std::max(rectangle.getLeft() / m_tileWidth, (NomadInteger)0);
    auto tileRight = std::min(rectangle.getRight() / m_tileWidth, m_tileMapWidth - 1);
    auto tileTop = std::max(rectangle.getTop() / m_tileHeight, (NomadInteger)0);
    auto tileBottom = std::min(rectangle.getBottom() / m_tileHeight, m_tileMapHeight - 1);

    for (NomadInteger y = tileTop; y <= tileBottom; ++y) {
        for (NomadInteger x = tileLeft; x <= tileRight; ++x) {
            auto tile_id = tile_layer.groundTileMap[y * m_tileMapWidth + x];
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

void Scene::setCameraPosition(const PointF &position) {
    m_cameraPosition = position;
}

void Scene::setCameraPosition(NomadFloat x, NomadFloat y) {
    cameraStopFollowEntity();

    setCameraPosition({x, y});
}

void Scene::setCameraX(NomadFloat x) {
    setCameraPosition(x, m_cameraPosition.getY());
}

void Scene::setCameraY(NomadFloat y) {
    setCameraPosition( m_cameraPosition.getX(), y);
}

void Scene::cameraStartFollowEntity(NomadId entity_id) {
    m_cameraFollowEntityId = entity_id;
}

void Scene::cameraStopFollowEntity() {
    m_cameraFollowEntityId = NOMAD_INVALID_ID;
}

PointF Scene::getCameraPosition() const {
    return m_cameraPosition;
}

NomadFloat Scene::getCameraX() const {
    return m_cameraPosition.getX();
}

NomadFloat Scene::getCameraY() const {
    return m_cameraPosition.getY();
}

NomadInteger Scene::getMaskAtEntity(const Entity* entity) const {
    return getMaskAtEntity(entity, entity->getLocation());
}

NomadInteger Scene::getMaskAtEntity(const Entity* entity, const PointF& location) const {
    auto layer = entity->getLayer();

    auto body_shape = entity->getBodyShape();

    if (body_shape == BodyShape::Rectangle) {
        auto width = entity->getBodyWidth();
        auto height = entity->getBodyHeight();
        auto x = location.getX() - width / 2;
        auto y = location.getY() - height / 2;

        RectangleF rectangle(x, y, width, height);

        return getMaskInRectangle(layer, rectangle, entity);
    } else if (body_shape == BodyShape::Circle) {
        auto x = location.getX();
        auto y = location.getY();
        auto radius = entity->getBodyRadius();

        CircleF circle(x, y, radius);

        return getMaskInCircle(layer, circle, entity);
    } else if (body_shape == BodyShape::None) {
        // No body shape, so no mask
        return 0;
    } else {
        log::warning("Unexpected body shape: " + std::to_string(static_cast<int>(body_shape)));
        return 0;
    }
}

NomadInteger Scene::getMaskFromEntitiesAt(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const {
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

NomadInteger Scene::getMaskFromEntitiesAt(NomadIndex layer, const CircleF& circle, const Entity* exclude) const {
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

NomadInteger Scene::getMaskInRectangle(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    NomadInteger mask = 0;

    mask |= getMaskFromEntitiesAt(layer, rectangle, exclude);

    Rectangle tile_area {
        static_cast<NomadInteger>(rectangle.getLeft()),
        static_cast<NomadInteger>(rectangle.getTop()),
        static_cast<NomadInteger>(rectangle.getRight()),
        static_cast<NomadInteger>(rectangle.getBottom())
    };

    processTilesAt(layer, tile_area, [&mask](const TileInformation& information) {
        mask |= information.mask;
    });

    return mask;
}

NomadInteger Scene::getMaskInCircle(NomadIndex layer, const CircleF& circle, const Entity* exclude) const {
    if (layer >= m_layers.size()) {
        return 0;
    }

    NomadInteger mask = 0;

    mask |= getMaskFromEntitiesAt(layer, circle, exclude);

    Rectangle tile_area {
        static_cast<NomadInteger>(circle.getX() - circle.getRadius()),
        static_cast<NomadInteger>(circle.getY() - circle.getRadius()),
        static_cast<NomadInteger>(circle.getX() + circle.getRadius()),
        static_cast<NomadInteger>(circle.getY() + circle.getRadius())
    };

    processTilesAt(layer, tile_area, [&mask](const TileInformation& information) {
        mask |= information.mask;
    });

    return mask;
}

void Scene::registerEntityEvent(const NomadString &name, NomadId entityId, NomadId scriptId) {
    auto registrations_it = std::find_if(
        m_entityEvents.begin(),
        m_entityEvents.end(),
        [&name](const auto& event) {
            return event.name == name;
        }
    );

    if (registrations_it == m_entityEvents.end()) {
        auto registration = std::vector<Event>();
        registration.emplace_back(entityId, scriptId);
        m_entityEvents.emplace_back(name, registration);
    } else {
        auto& registrations = registrations_it->registrations;

        auto it = std::find_if(
            registrations_it->registrations.begin(),
            registrations_it->registrations.end(),
            [entityId](const Event& registration) {
                return registration.entityId == entityId;
            }
        );

        if (it == registrations.end()) {
            registrations.emplace_back(entityId, scriptId);
        } else {
            it->scriptId = scriptId;
        }
    }
}

void Scene::unregisterEntityEvent(const NomadString &name, NomadId entityId) {
    auto events_it = std::find_if(
        m_entityEvents.begin(),
        m_entityEvents.end(),
        [&name](const auto& event) {
            return event.name == name;
        }
    );

    if (events_it != m_entityEvents.end()) {
        auto& registrations = events_it->registrations;

        auto it = std::remove_if(
            registrations.begin(),
            registrations.end(),
            [entityId](const Event& registration) {
                return registration.entityId == entityId;
            }
        );

        registrations.erase(it, registrations.end());
    } else {
        log::warning("Entity ID " + toString(entityId) + " requested to unregister event '" + name + "' but no such event was found");
    }
}

void Scene::unregisterEntityFromAllEvents(NomadId entityId) {
    for (auto&[name, registrations] : m_entityEvents) {
        auto it = std::ranges::remove_if(
            registrations,
            [entityId](const Event& registration) {
                return registration.entityId == entityId;
            }
        ).begin();

        registrations.erase(it, registrations.end());
    }
}

void Scene::triggerEvent(const NomadString &name) {
    auto events_it = std::ranges::find_if(m_entityEvents,
        [&name](const auto& event) {
            return event.name == name;
        }
    );

    if (events_it != m_entityEvents.end()) {
        auto& registrations = events_it->registrations;

        for (auto& registration : registrations) {
            const auto entity = getEntityById(registration.entityId);

            if (entity) {
                m_game->executeScriptInNewContext(registration.scriptId, this, entity);
            }
        }
    } else {
        log::warning("Event '" + name + "' not found");
    }
}

void Scene::triggerEvent(const NomadString &name, Entity *entity) {
    auto events_it = std::ranges::find_if(
        m_entityEvents,
        [&name](const auto& event) {
            return event.name == name;
        }
    );

    if (events_it == m_entityEvents.end()) {
        log::warning("Event '" + name + "' not found");
        return;
    }

    auto& registrations = events_it->registrations;

    for (auto& registration : registrations) {
        if (registration.entityId == entity->getId()) {
            m_game->executeScriptInNewContext(registration.scriptId, this, entity);
            return;
        }
    }
}

void Scene::triggerEventLayer(const NomadString &name, NomadIndex layerId) {
    auto events_it = std::find_if(
        m_entityEvents.begin(),
        m_entityEvents.end(),
        [&name](const auto& event) {
            return event.name == name;
        }
    );

    if (events_it != m_entityEvents.end()) {
        auto& registrations = events_it->registrations;

        for (auto& registration : registrations) {
            const auto entity = getEntityById(registration.entityId);

            if (entity && entity->getLayer() == layerId) {
                m_game->executeScriptInNewContext(registration.scriptId, this, entity);
            }
        }
    } else {
        log::warning("Event '" + name + "' not found");
    }
}

void Scene::scheduleEvent(const NomadString &name, NomadInteger frameCount) {
    auto current_frame = getFrameNumber();

    m_scheduledEvents.emplace_back(ScheduledEventRegistration{
        name,
        current_frame + frameCount,
    });

    std::sort(m_scheduledEvents.begin(), m_scheduledEvents.end(),
        [](const ScheduledEventRegistration& a, const ScheduledEventRegistration& b) {
            return a.frameNumber < b.frameNumber;
        }
    );
}

void Scene::forEachEntities(const std::function<void(Entity*)>& callback) const {
    for (auto entity : m_entities) {
        callback(entity);
    }
}

void Scene::forEachEntityByLayer(NomadIndex layerIndex, const std::function<void(Entity*)>& callback) const {
    if (layerIndex >= m_layers.size()) {
        return;
    }

    auto& layer = m_layers[layerIndex];

    for (auto entity : layer.entities) {
        callback(entity);
    }
}

} // nomad