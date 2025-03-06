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

    [[nodiscard]] Game* get_game() const;
    void set_game(Game* game);;

    void set_name(const NomadString& name);;
    [[nodiscard]] NomadString get_name() const;;

    void set_z(NomadInteger z);
    [[nodiscard]] NomadInteger get_z() const;;

    void update(Game* game);
    void render(Canvas* canvas);
    void process_input_event(const InputEvent& event);

    void set_variable_value(NomadId variable_id, const ScriptValue& value);
    void get_variable_value(NomadId variable_id, ScriptValue& value) const;

    void create_entity(
        const NomadString& init_script_name,
        NomadFloat x,
        NomadFloat y,
        NomadInteger layer,
        NomadId id = NOMAD_INVALID_ID,
        const NomadString& text = NOMAD_EMPTY_STRING);
    void remove_entity(Entity* entity);

    [[nodiscard]] Entity* get_entity_by_id(NomadId id) const;

    [[nodiscard]] Entity* get_entity_by_name(const NomadString& name) const;
    void get_entities_by_name(const NomadString& name, EntityList& entities) const;

    void add_event(const NomadString& name, NomadId script_id);
    void remove_event(const NomadString& name);

    void load_action_mapping(const NomadString& mapping_name);
    void save_action_mapping(const NomadString& mapping_name);
    void reset_action_mapping(const NomadString& mapping_name);

    void add_action_pressed(const NomadString& action_name, NomadId script_id, NomadId entity_id = NOMAD_INVALID_ID);
    void add_action_released(const NomadString& action_name, NomadId script_id, NomadId entity_id = NOMAD_INVALID_ID);

    void remove_action_pressed(const NomadString& action_name, NomadId entity_id = NOMAD_INVALID_ID);
    void remove_action_released(const NomadString& action_name, NomadId entity_id = NOMAD_INVALID_ID);

    // Tile map
    void load_tile_map(const NomadString& file_name, const NomadString& tile_set_texture_name);

    void set_tile_set(const Texture* texture, NomadInteger tile_width, NomadInteger tile_height, NomadIndex first_tile_index);
    void set_tile_mask(NomadIndex tile_index, NomadInteger tile_mask);
    [[nodiscard]] NomadInteger get_tile_mask(NomadIndex tile_index) const;

    void set_tile_map_size(NomadInteger width, NomadInteger height);
    [[nodiscard]] NomadInteger get_tile_map_width() const;
    [[nodiscard]] NomadInteger get_tile_map_height() const;

    void set_ground_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index);
    void set_wall_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y, NomadIndex tile_index);
    void set_wall_mask(NomadInteger mask);
    [[nodiscard]] NomadInteger get_wall_mask() const;
    [[nodiscard]] NomadIndex get_ground_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y) const;
    [[nodiscard]] NomadIndex get_wall_tile_index(NomadIndex layer, NomadInteger x, NomadInteger y) const;
    [[nodiscard]] NomadInteger get_tile_mask(NomadIndex layer, NomadInteger x, NomadInteger y) const;

    void process_tiles_at(NomadIndex layer, const Rectangle& rectangle, TileCallback callback) const;

    // Camera
    void set_camera_position(const PointF& position);
    void set_camera_position(NomadFloat x, NomadFloat y);
    void set_camera_x(NomadFloat x);
    void set_camera_y(NomadFloat y);
    void camera_start_follow_entity(NomadId entity_id);
    void camera_stop_follow_entity();

    [[nodiscard]] PointF get_camera_position() const;
    [[nodiscard]] NomadFloat get_camera_x() const;
    [[nodiscard]] NomadFloat get_camera_y() const;

    // Mask
    [[nodiscard]] NomadInteger get_mask_at_entity(const Entity* entity) const;
    [[nodiscard]] NomadInteger get_mask_at_entity(const Entity* entity, const PointF& location) const;
    [[nodiscard]] NomadInteger get_mask_from_entities_at(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger get_mask_from_entities_at(NomadIndex layer, const CircleF& circle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger get_mask_in_rectangle(NomadIndex layer, const RectangleF& rectangle, const Entity* exclude) const;
    [[nodiscard]] NomadInteger get_mask_in_circle(NomadIndex layer, const CircleF& circle, const Entity* exclude) const;

    // Events
    void register_entity_event(const NomadString& name, NomadId entity_id, NomadId script_id);
    void unregister_entity_event(const NomadString& name, NomadId entity_id);
    void unregister_entity_all_events(NomadId entity_id);
    void trigger_event(const NomadString& name);
    void trigger_event(const NomadString& name, Entity* entity);
    void trigger_event_layer(const NomadString& name, NomadIndex layer_id);

    // Entity iteration
    void for_each_entities(const std::function<void(Entity*)>& callback) const;
    void for_each_entity_by_layer(NomadIndex layer_index, const std::function<void(Entity*)>& callback) const;

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
        std::vector<NomadIndex> ground_tile_map;
        bool has_ground_tile_map = false;
        std::vector<NomadIndex> wall_tile_map;
        bool has_wall_tile_map = false;
        std::vector<b2BodyId> walls;
        bool walls_invalidated = true;

        b2WorldId world_id;  // Initialized by Scene
    };

    struct ActionMapping {
        NomadString name;
        InputAction action = InputAction::Unknown;
        NomadId script_id = NOMAD_INVALID_ID;
        NomadId entity_id = NOMAD_INVALID_ID;
        bool pressed = false;
        bool released = false;
        bool held = false;
    };

    struct AddedEntity {
        NomadId init_script_id;
        NomadFloat x, y;
        NomadInteger layer;
        NomadId id;
        NomadString text_id;
    };

    struct Event {
        NomadId entity_id;
        NomadId script_id;
    };

    struct EventRegistrations {
        // Event name
        NomadString name;
        std::vector<Event> registrations;
    };

private: // methods
    NomadId get_next_entity_id();
    ActionMapping* get_action_mapping(const NomadString& name, InputAction type, NomadId entity_id = NOMAD_INVALID_ID);

    bool load_tile_map_layer(
        const NomadString& ground_layer_name,
        NomadInteger tile_map_height,
        NomadInteger tile_map_width,
        boost::json::array& layers,
        std::vector<NomadIndex>& tile_map
    ) const;

    void update_physics();
    void update_entity_layers();
    void update_camera();

    void render_tile_map(Canvas* canvas, const Layer& layer);
    void render_tile(Canvas* canvas, int y, int x, NomadIndex ground_tile_index);

    void process_add_remove_entities();

private: // data
    Game* m_game;
    NomadString m_name;
    NomadInteger m_z = 0;

    VariableList m_variables;

    // Entities
    NomadId m_entity_id_counter = NOMAD_ID_MIN;

    std::vector<AddedEntity> m_added_entities;
    EntityList m_removed_entities;
    EntityList m_entities;

    // Tiles
    const Texture* m_tile_texture = nullptr;
    NomadInteger m_tile_count = 0;
    NomadInteger m_tile_width = 0, m_tile_height = 0;
    NomadInteger m_tile_map_width = 0, m_tile_map_height = 0;
    b2Filter m_wall_filter = b2DefaultFilter();
    std::vector<TileDefinition> m_tiles;
    std::array<Layer, SCENE_LAYER_COUNT> m_layers;

    // Camera
    PointF m_camera_position;
    NomadId m_camera_follow_entity_id = NOMAD_INVALID_ID;

    // Events
    std::vector<EventRegistrations> m_entity_events;

    EventManager m_events;
    ActionManager m_action_manager;
    std::vector<ActionMapping> m_action_mapping;
    GameExecutionContext m_execution_context;
};

} // nomad

