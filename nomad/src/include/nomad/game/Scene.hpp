//
// Created by jfbilodeau on 23-06-14.
//

#pragma once

#include "nomad/game/ActionManager.hpp"
#include "nomad/game/Entity.hpp"
#include "nomad/game/Event.hpp"

#include "nomad/geometry/Rectangle.hpp"

#include "box2d/box2d.h"
#include "boost/json/array.hpp"

#include <array>
#include <functional>
#include <vector>

namespace nomad {

constexpr NomadInteger SCENE_LAYER_COUNT = 5;

// Forward declarations
class Canvas;
class Game;

struct TileInformation {
    NomadInteger x;
    NomadInteger y;
    NomadIndex id;
    NomadInteger mask;
};

using TileCallback = std::function<void(const TileInformation&)>;

class Scene {
public:
    explicit Scene(Game* game);
    Scene(const Scene& other) = delete;
    ~Scene();

    [[nodiscard]] Game* getGame() const;
    void setGame(Game* game);

    void setName(const NomadString& name);
    [[nodiscard]] NomadString getName() const;

    void setZ(NomadInteger z);
    [[nodiscard]] NomadInteger getZ() const;

    void setFrameNumber(NomadInteger frameNumber);
    NomadInteger getFrameNumber() const;

    void update(Game* game);
    void render(Canvas* canvas);
    void processInputEvent(const InputEvent& event);

    void setVariableValue(NomadId variableId, const ScriptValue& value);
    void getVariableValue(NomadId variableId, ScriptValue& value) const;

    void createEntity(
        const NomadString& initScriptName,
        NomadFloat x,
        NomadFloat y,
        NomadInteger layer,
        NomadId id = NOMAD_INVALID_ID,
        const NomadString& text = NOMAD_EMPTY_STRING);
    void removeEntity(Entity* entity);

    [[nodiscard]]
    Entity* getEntityById(NomadId id) const;

    [[nodiscard]]
    Entity* getEntityByName(const NomadString& name) const;
    void getEntitiesByName(const NomadString& name, EntityList& entities) const;

    void pauseOtherEntities(Entity* entity);
    void pauseOtherEntities(const std::vector<Entity*>& entities);
    void pauseAllEntities();
    void unpauseAllEntities();
    void unpauseAllVisibleEntities();

    void addEvent(const NomadString& name, NomadId scriptId);
    void removeEvent(const NomadString& name);

    void loadActionMapping(const NomadString& mappingName);
    void saveActionMapping(const NomadString& mappingName);
    void resetActionMapping(const NomadString& mappingName);

    void addActionPressed(const NomadString& actionName, NomadId scriptId, NomadId entityId = NOMAD_INVALID_ID);
    void addActionReleased(const NomadString& actionName, NomadId scriptId, NomadId entityId = NOMAD_INVALID_ID);

    void removeActionPressed(const NomadString& actionName, NomadId entityId = NOMAD_INVALID_ID);
    void removeActionReleased(const NomadString& actionName, NomadId entityId = NOMAD_INVALID_ID);

    // Tile map
    void loadTileMap(const NomadString& fileName, const NomadString& tileSetTextureName);

    void setTileSet(const Texture* texture, NomadInteger tileWidth, NomadInteger tileHeight, NomadIndex firstTileIndex);
    void setTileMask(NomadIndex tileIndex, NomadInteger tileMask);
    [[nodiscard]] NomadInteger getTileMask(NomadIndex tileIndex) const;

    void setTileMapSize(NomadInteger width, NomadInteger height);
    [[nodiscard]] NomadInteger getTileMapWidth() const;
    [[nodiscard]] NomadInteger getTileMapHeight() const;

    void setGroundTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index);
    void setWallTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index);
    void setWallMask(NomadInteger mask);
    [[nodiscard]] NomadInteger getWallMask() const;
    [[nodiscard]] NomadIndex getGroundTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y) const;
    [[nodiscard]] NomadIndex getWallTileIndex(NomadIndex layer, NomadInteger x, NomadInteger y) const;
    [[nodiscard]] NomadInteger getTileMask(NomadIndex layer, NomadInteger x, NomadInteger y) const;

    void processTilesAt(NomadIndex layer, const Rectangle& rectangle, TileCallback callback) const;

    // Camera
    void setCameraPosition(const PointF& position);
    void setCameraPosition(NomadFloat x, NomadFloat y);
    void setCameraX(NomadFloat x);
    void setCameraY(NomadFloat y);
    void cameraStartFollowEntity(NomadId entity_id);
    void cameraStopFollowEntity();

    [[nodiscard]] PointF getCameraPosition() const;
    [[nodiscard]] NomadFloat getCameraX() const;
    [[nodiscard]] NomadFloat getCameraY() const;

    // Mask
    [[nodiscard]] NomadInteger getMaskAtEntity(const Entity* entity) const;
    [[nodiscard]] NomadInteger getMaskAtEntity(const Entity* entity, const PointF& location) const;
    [[nodiscard]] NomadInteger getMaskFromEntitiesAt(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger getMaskFromEntitiesAt(NomadIndex layer, const CircleF& circle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger getMaskInRectangle(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger getMaskInCircle(NomadIndex layer, const CircleF& circle, const Entity* exclude) const;

    // Events
    void registerEntityEvent(const NomadString& name, NomadId entityId, NomadId scriptId);
    void unregisterEntityEvent(const NomadString& name, NomadId entityId);
    void unregisterEntityAllEvents(NomadId entityId);
    void triggerEvent(const NomadString& name);
    void triggerEvent(const NomadString& name, Entity* entity);
    void triggerEventLayer(const NomadString& name, NomadIndex layerId);
    void scheduleEvent(const NomadString& name, NomadInteger frameCount);

    // Entity iteration
    void forEachEntities(const std::function<void(Entity*)>& callback) const;
    void forEachEntityByLayer(NomadIndex layerIndex, const std::function<void(Entity*)>& callback) const;

private: // structs
    struct TileDefinition {
        NomadInteger mask = 0;
        Rectangle source;
    };

    struct Layer {
        Layer() = default;
        // Discovered a bug where I copied layer instead of referencing them. Let's make sure that doesn't happen again...
        Layer(const Layer& other) = delete;
        Layer& operator=(const Layer& other) = delete;

        NomadId id = NOMAD_INVALID_ID;
        EntityList entities;
        std::vector<NomadIndex> groundTileMap;
        bool hasGroundTileMap = false;
        std::vector<NomadIndex> wallTileMap;
        bool hasWallTileMap = false;
        std::vector<b2BodyId> walls;
        bool wallsInvalidated = true;

        b2WorldId worldId;  // Initialized by Scene
    };

    struct ActionMapping {
        NomadString name;
        InputAction action = InputAction::Unknown;
        NomadId scriptId = NOMAD_INVALID_ID;
        NomadId entityId = NOMAD_INVALID_ID;
        bool pressed = false;
        bool released = false;
        bool held = false;
    };

    struct AddedEntity {
        NomadId initScriptId;
        NomadFloat x, y;
        NomadInteger layer;
        NomadId id;
        NomadString textId;
    };

    struct Event {
        NomadId entityId;
        NomadId scriptId;
    };

    struct EventRegistrations {
        // Event name
        NomadString name;
        std::vector<Event> registrations;
    };

    struct ScheduledEventRegistration {
        NomadString name;
        NomadInteger frameNumber;
    };

private: // methods
    NomadId getNextEntityId();
    ActionMapping* getActionMapping(const NomadString& name, InputAction type, NomadId entityId = NOMAD_INVALID_ID);

    bool loadTileMapLayer(
        const NomadString& groundLayerName,
        NomadInteger tileMapHeight,
        NomadInteger tileMapWidth,
        boost::json::array& layers,
        std::vector<NomadIndex>& tileMap
    ) const;

    void updateScheduledEvents();
    void updatePhysics();
    void updateEntityLayers();
    void updateCamera();

    void processRemoveEntities();
    void processAddEntities();

    void renderTileMap(Canvas* canvas, const Layer& layer);
    void renderTile(Canvas* canvas, int y, int x, NomadIndex groundTileIndex);

private: // data
    Game* m_game;
    NomadString m_name;
    NomadInteger m_z = 0;
    NomadInteger m_frameNumber = 0;

    VariableList m_variables;

    // Entities
    NomadId m_entityIdCounter = NOMAD_ID_MIN;

    std::vector<AddedEntity> m_addedEntities;
    EntityList m_removedEntities;
    EntityList m_entities;

    // Tiles
    const Texture* m_tileTexture = nullptr;
    NomadInteger m_tileCount = 0;
    NomadInteger m_tileWidth = 0, m_tileHeight = 0;
    NomadInteger m_tileMapWidth = 0, m_tileMapHeight = 0;
    b2Filter m_wallFilter = b2DefaultFilter();
    std::vector<TileDefinition> m_tiles;
    std::array<Layer, SCENE_LAYER_COUNT> m_layers;

    // Camera
    PointF m_cameraPosition;
    NomadId m_cameraFollowEntityId = NOMAD_INVALID_ID;

    // Events
    std::vector<EventRegistrations> m_entityEvents;
    std::vector<ScheduledEventRegistration> m_scheduledEvents;

    EventManager m_events;
    ActionManager m_actionManager;
    std::vector<ActionMapping> m_actionMapping;
    GameExecutionContext m_executionContext;
};

} // nomad

