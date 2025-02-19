//
// Created by jfbilodeau on 23-06-14.
//

#ifndef NOMAD_ENTITY_H
#define NOMAD_ENTITY_H

#include "nomad/nomad.hpp"

#include "nomad/game/Alignment.hpp"
#include "nomad/game/Body.hpp"
#include "nomad/game/Cardinal.hpp"
#include "nomad/game/Color.hpp"
#include "nomad/game/Event.hpp"
#include "nomad/game/GameExecutionContext.hpp"

#include "nomad/geometry/PointF.hpp"
#include "nomad/geometry/RectangleF.hpp"

#include "box2d/box2d.h"

namespace nomad {

// Forward declarations
class Animation;
class Canvas;
class Game;
class Scene;
class Sprite;
class Texture;

class Entity {
public:
    explicit Entity(Scene* scene, const VariableMap* variableMap, NomadId id, NomadFloat x, NomadFloat y, NomadInteger layer);
    Entity(const Entity& other) = delete;
    ~Entity();

//    void init(Game* game, NomadId init_script_id);
    void update(Scene* scene);
    void render(Canvas* canvas);

    [[nodiscard]] NomadId get_id() const;

    [[nodiscard]] Scene* get_scene() const;

    void set_name(const NomadString& name);
    [[nodiscard]] const NomadString& get_name() const;

    void set_script_name(const NomadString& script_name);
    [[nodiscard]] const NomadString& get_script_name() const;

    void set_visible(NomadBoolean visible);
    [[nodiscard]] NomadBoolean is_visible() const;

    void set_x(NomadFloat x);
    [[nodiscard]] NomadFloat get_x() const;

    void set_y(NomadFloat y);
    [[nodiscard]] NomadFloat get_y() const;

    void set_z(NomadFloat z);
    [[nodiscard]] NomadFloat get_z() const;

    void set_location(NomadFloat x, NomadFloat y);
    void set_location(const PointF& location);
    [[nodiscard]] const PointF& get_location() const;

    void stop_moving();
    void start_moving(const PointF& velocity);
    void start_moving(NomadFloat x, NomadFloat y);
    void start_moving_in_direction(NomadFloat angle, NomadFloat speed);
    void start_moving_to(const PointF& destination, NomadFloat speed);
    void start_moving_to(NomadFloat x, NomadFloat y, NomadFloat speed);

    void set_velocity(NomadFloat x, NomadFloat y);
    void set_velocity(const PointF& velocity);
    void set_velocity(Cardinal direction, NomadFloat speed);
    void set_velocity_x(NomadFloat x);
    void set_velocity_y(NomadFloat y);
    [[nodiscard]] const PointF& get_velocity() const;

    void set_destination(NomadFloat x, NomadFloat y);
    void set_destination(const PointF& destination);
    [[nodiscard]] const PointF& get_destination() const;

    void set_destination_x(NomadFloat x);
    [[nodiscard]] NomadFloat get_destination_x() const;

    void set_destination_y(NomadFloat y);
    [[nodiscard]] NomadFloat get_destination_y() const;

    void set_speed(NomadFloat speed);
    [[nodiscard]] NomadFloat get_speed() const;

    void set_mask(NomadInteger mask);
    [[nodiscard]] NomadInteger get_mask() const;

    void set_collision_mask(NomadInteger collision_mask);
    [[nodiscard]] NomadInteger get_collision_mask() const;

    void set_sensor(bool is_sensor);
    [[nodiscard]] bool is_sensor() const;

    void set_no_body();
    void set_circle_body(BodyType body_type, NomadFloat radius);
    void set_rectangle_body(BodyType body_type, NomadFloat width, NomadFloat height);

    [[nodiscard]] BodyType get_body_type() const;
    [[nodiscard]] BodyShape get_body_shape() const;
    [[nodiscard]] NomadFloat get_body_width() const;
    [[nodiscard]] NomadFloat get_body_height() const;
    [[nodiscard]] NomadFloat get_body_radius() const;

    [[nodiscard]] bool is_touching(const RectangleF& rectangle) const;
    [[nodiscard]] bool is_touching(const CircleF& circle) const;
    [[nodiscard]] bool is_touching(const Entity* entity) const;

    RectangleF& get_bounding_box(RectangleF& rectangle) const;

    // Notify the entity that it has entered the camera frame.
    void enter_camera();
    // Notify the entity that it has exited the camera frame.
    void exit_camera();

    // Script to execute when the entity enters the camera frame.
    void set_on_enter_camera(NomadId script_id);
    [[nodiscard]] NomadId get_on_enter_camera() const;

    // Script to execute when the entity exits the camera frame.
    void set_on_exit_camera(NomadId script_id);
    [[nodiscard]] NomadId get_on_exit_camera() const;

    [[nodiscard]] bool is_in_camera() const;

    void invalidate_physics_body();
    void before_simulation_update(b2WorldId world);
    void after_simulation_update(b2WorldId world);

    void set_layer(NomadInteger layer);
    [[nodiscard]] NomadInteger get_layer() const;

    void set_sprite_name(const NomadString& sprite_name);
    [[nodiscard]] const NomadString& get_sprite_name() const;

    void set_sprite(const Sprite* sprite);
    [[nodiscard]] const Sprite* get_sprite() const;

    void set_sprite_x(NomadFloat x);
    [[nodiscard]] NomadFloat get_sprite_x() const;

    void set_sprite_y(NomadFloat y);
    [[nodiscard]] NomadFloat get_sprite_y() const;

    void set_sprite_anchor(const PointF& anchor);
    void set_sprite_anchor(NomadFloat x, NomadFloat y);
    [[nodiscard]] const PointF& get_sprite_anchor() const;

    void set_animation(const Animation* animation);
    [[nodiscard]] const Animation* get_animation() const;

    void set_animation_name(const NomadString& animation_name);
    [[nodiscard]] const NomadString& get_animation_name() const;

    void set_animation_variant(const NomadString& animation_variant);
    [[nodiscard]] const NomadString& get_animation_variant() const;

    void set_animation_direction(const NomadString& animation_direction);
    [[nodiscard]] const NomadString& get_animation_direction() const;

    void set_animation_duration(NomadInteger speed);
    [[nodiscard]] NomadInteger get_animation_duration() const;

    void set_animation_repeat(bool repeat);
    [[nodiscard]] bool get_animation_repeat() const;

    void set_animation_reverse(bool reverse);
    [[nodiscard]] bool get_animation_reverse() const;

    void set_text(const NomadString& text);
    [[nodiscard]] const NomadString& get_text();

    void set_text_alignment(Alignment alignment);
    [[nodiscard]] Alignment get_text_alignment() const;

    void set_text_position(NomadFloat x, NomadFloat y);
    void set_text_position(const PointF& position);
    [[nodiscard]] const PointF& get_text_position() const;

    void set_text_x(NomadFloat x);
    [[nodiscard]] NomadFloat get_text_x() const;

    void set_text_y(NomadFloat y);
    [[nodiscard]] NomadFloat get_text_y() const;

    void set_text_width(NomadFloat width);
    [[nodiscard]] NomadFloat get_text_width() const;

    void set_text_height(NomadFloat height);
    [[nodiscard]] NomadFloat get_text_height() const;

    void set_text_line_spacing(NomadFloat line_spacing);
    [[nodiscard]] NomadFloat get_text_line_spacing() const;

    void set_text_color(const Color& color);
    [[nodiscard]] Color get_text_color() const;

    void set_font_by_id(NomadId font_id);
    [[nodiscard]] NomadId get_font_id() const;

    void set_variable_value(NomadId variable_id, const ScriptValue& value);
    void get_variable_value(NomadId variable_id, ScriptValue& value) const;

    void add_event(const NomadString& name, NomadId script_id);
    void remove_event(const NomadString& name);

    GameExecutionContext* get_execution_context();

    [[nodiscard]] NomadId get_on_frame() const;
    void set_on_frame(NomadId script_id);

    [[nodiscard]] NomadId get_on_collision_begin() const;
    void set_on_collision_begin(NomadId script_id);

    [[nodiscard]] NomadId get_on_collision_end() const;
    void set_on_collision_end(NomadId script_id);

private:
    NomadId m_id = NOMAD_INVALID_ID;
    Scene* m_scene;
    NomadString m_script_name;
    NomadString m_name;
    PointF m_velocity = {};
    bool m_move_to_destination = false;
    PointF m_destination = {};
    NomadFloat m_speed = 0;

    // World position
    PointF m_position;
    NomadFloat m_z = 0.0;
    NomadInteger m_layer = 0;

    // Mask, body and collision
    BodyShape m_body_shape = BodyShape::None;
    BodyType m_body_type = BodyType::Static;
    NomadBoolean m_is_sensor = false;
    NomadFloat m_body_width = 0;
    NomadFloat m_body_height = 0;
    NomadFloat m_body_radius = 0;
    b2BodyId m_b2_body = {};
    b2ShapeId m_b2_shape = {};
    b2Filter m_b2_filter = b2DefaultFilter();
    NomadBoolean m_has_body = false;
    NomadBoolean m_body_invalidated = true;
    NomadBoolean m_position_invalidated = true;
    NomadBoolean m_velocity_invalidated = true;

    // Camera
    bool m_in_camera = false;

    // Visuals
    NomadBoolean m_visible = true;
    PointF m_sprite_anchor;  // Sprite image anchor point
    const Sprite* m_sprite = nullptr;
    const Animation* m_animation = nullptr;
    NomadInteger m_current_frame = 0;
    NomadInteger m_frame_count = 0;

    NomadString m_animation_name;
    NomadString m_animation_variant = "idle";
    NomadString m_animation_direction = "south";
    NomadInteger m_animation_duration = 1;
    NomadBoolean m_animation_repeat = true;
    NomadBoolean m_animation_reverse = false;
    bool m_animation_dirty = false;

    // Text
    NomadString m_text;
    NomadString m_wrapped_text;
    Alignment m_text_alignment = Alignment::CenterMiddle;
    PointF m_text_position;
    NomadFloat m_text_width = 0;
    NomadFloat m_text_height = 0;
    NomadFloat m_text_line_spacing = 0;
    NomadId m_font_id = NOMAD_INVALID_ID;
    Color m_text_color = Colors::Black;
    SDL_Texture* m_text_texture = nullptr;

    VariableList m_variables;
    EventManager m_events;
    GameExecutionContext m_execution_context;

    // Events
    NomadId m_on_frame = NOMAD_INVALID_ID;
    NomadId m_on_collision_begin = NOMAD_INVALID_ID;
    NomadId m_on_collision_end = NOMAD_INVALID_ID;
    NomadId m_on_enter_camera = NOMAD_INVALID_ID;
    NomadId m_on_exit_camera = NOMAD_INVALID_ID;

    void invalidate_text_texture();
    void generate_text_texture(Canvas* canvas);
};

using EntityList = std::vector<Entity*>;

} // nomad
#include "box2d/box2d.h"

#endif //NOMAD_ENTITY_H
