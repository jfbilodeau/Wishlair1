//
// Created by jfbilodeau on 23-06-14.
//

#include "nomad/game/Entity.hpp"

#include "nomad/game/Canvas.hpp"
#include "nomad/game/Event.hpp"
#include "nomad/game/Game.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/geometry/Intersection.hpp"

#include "nomad/resource/Animation.hpp"
#include "nomad/resource/Font.hpp"
#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Sprite.hpp"

#include <sstream>

namespace nomad {

Entity::Entity(Scene* scene, const VariableMap* variableMap, NomadId id, NomadFloat x, NomadFloat y, NomadInteger layer):
    m_scene(scene),
    m_variables(variableMap),
    m_id(id),
    m_position(x, y),
    m_layer(layer)
{
    m_execution_context.reset(scene, this);
}

Entity::~Entity() {
    invalidate_text_texture(); // Clear the text texture if it exists
}

Scene* Entity::get_scene() const {
    return m_scene;
}

void Entity::set_name(const NomadString& name) {
    m_name = name;
}

const NomadString& Entity::get_name() const {
    return m_name;
}

void Entity::set_script_name(const NomadString& script_name) {
    m_script_name = script_name;
}

const NomadString& Entity::get_script_name() const {
    return m_script_name;
}

void Entity::set_x(const NomadFloat x) {
    m_position.set_x(x);
    m_destination.set_x(x);
}

NomadFloat Entity::get_x() const {
    return m_position.x();
}

void Entity::set_y(NomadFloat y) {
    m_position.set_y(y);
    m_destination.set_y(y);
}

NomadFloat Entity::get_y() const {
    return m_position.y();
}

void Entity::set_z(NomadFloat z) {
    m_z = z;
}

NomadFloat Entity::get_z() const {
    return m_z;
}

void Entity::set_location(NomadFloat x, NomadFloat y) {
    m_position.set(x, y);
}

void Entity::set_location(const PointF& location) {
    m_position.set(location);
}

const PointF& Entity::get_location() const {
    return m_position;
}

void Entity::stop_moving() {
   m_velocity.zero();
}

void Entity::start_moving(const PointF& velocity) {
    m_velocity.set(velocity);
}

void Entity::start_moving(NomadFloat x, NomadFloat y) {
    m_velocity.set(x, y);
}

void Entity::start_moving_in_direction(NomadFloat angle, NomadFloat speed) {
    m_velocity.set(
        std::cos(angle) * speed,
        std::sin(angle) * speed
    );
}

void Entity::start_moving_to(const PointF& destination, NomadFloat speed) {
    start_moving_to(destination.x(), destination.y(), speed);
}

void Entity::start_moving_to(NomadFloat x, NomadFloat y, NomadFloat speed) {
    m_destination.set(x, y);
    m_speed = speed;
}

void Entity::set_velocity(NomadFloat x, NomadFloat y) {
    set_velocity({x, y});
}

void Entity::set_velocity(const PointF& velocity) {
    m_velocity.set(velocity);
}

void Entity::set_velocity(Cardinal direction, NomadFloat speed) {
    switch (direction) {
        case Cardinal::North:
            m_velocity.set(0.0, -speed);
            break;

        case Cardinal::East:
            m_velocity.set(speed, 0.0);
            break;

        case Cardinal::South:
            m_velocity.set(0.0, speed);
            break;

        case Cardinal::West:
            m_velocity.set(-speed, 0.0);
            break;

        case Cardinal::Unknown:
            m_velocity.set(0, 0);
            break;
    }
}

void Entity::set_velocity_x(NomadFloat x) {
    m_velocity.set_x(x);
}

void Entity::set_velocity_y(NomadFloat y) {
    m_velocity.set_y(y);
}

const PointF& Entity::get_velocity() const {
    return m_velocity;
}

void Entity::set_destination(NomadFloat x, NomadFloat y) {
    m_destination.set(x, y);
}

void Entity::set_destination(const PointF& destination) {
    m_destination.set(destination);
}

const PointF& Entity::get_destination() const {
    return m_destination;
}

void Entity::set_destination_x(NomadFloat x) {
    m_destination.set_x(x);
}

NomadFloat Entity::get_destination_x() const {
    return m_destination.x();
}

void Entity::set_destination_y(NomadFloat y) {
    m_destination.set_y(y);
}

NomadFloat Entity::get_destination_y() const {
    return m_destination.y();
}

void Entity::set_speed(NomadFloat speed) {
    m_speed = speed;
}

NomadFloat Entity::get_speed() const {
    return m_speed;
}

void Entity::set_mask(const NomadInteger mask) {
//    m_mask = mask;
    m_b2_filter.categoryBits = mask;
}

NomadInteger Entity::get_mask() const {
    return m_b2_filter.categoryBits;
}

void Entity::set_collision_mask(const NomadInteger collision_mask) {
//    m_collision_mask = collision_mask;
    m_b2_filter.maskBits = collision_mask;
}

[[nodiscard]] NomadInteger Entity::get_collision_mask() const {
    return m_b2_filter.maskBits;
}

void Entity::set_no_body() {
    invalidate_body();

    m_body_shape = BodyShape::None;
}

void Entity::set_circle_body(BodyType body_type, NomadFloat radius) {
    invalidate_body();

    m_body_shape = BodyShape::Circle;
    m_body_type = body_type;
    m_body_radius = radius;
}

void Entity::set_rectangle_body(BodyType body_type, NomadFloat width, NomadFloat height) {
    invalidate_body();

    m_body_shape = BodyShape::Rectangle;
    m_body_type = body_type;
    m_body_width = width;
    m_body_height = height;
}

BodyType Entity::get_body_type() const {
    return m_body_type;
}

BodyShape Entity::get_body_shape() const {
    return m_body_shape;
}

NomadFloat Entity::get_body_width() const {
    return m_body_width;
}

NomadFloat Entity::get_body_height() const {
    return m_body_height;
}

NomadFloat Entity::get_body_radius() const {
    return m_body_radius;
}

bool Entity::is_touching(const RectangleF& rectangle) const {
    if (m_body_shape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            m_position.x() - m_body_width / 2,
            m_position.y() - m_body_height / 2,
            m_body_width,
            m_body_height
        );

        return rectangle_rectangle_intersect(entity_rectangle, rectangle);
    } else if (m_body_shape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            m_position.x(),
            m_position.y(),
            m_body_radius
        );

        return circle_rectangle_intersect(entity_circle, rectangle);
    }

    return false;
}

bool Entity::is_touching(const CircleF& circle) const {
    if (m_body_shape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            m_position.x() - m_body_width / 2,
            m_position.y() - m_body_height / 2,
            m_body_width,
            m_body_height
        );

        return circle_rectangle_intersect(circle, entity_rectangle);
    } else if (m_body_shape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            m_position.x(),
            m_position.y(),
            m_body_radius
        );

        return circle_circle_intersect(entity_circle, circle);
    }

    return false;
}

bool Entity::is_touching(const Entity* entity) const {
    auto entity_body_shape = entity->get_body_shape();

    if (entity_body_shape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            entity->get_x() - entity->get_body_width() / 2,
            entity->get_y() - entity->get_body_height() / 2,
            entity->get_body_width(),
            entity->get_body_height()
        );

        return is_touching(entity_rectangle);
    } else if (entity_body_shape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            entity->get_x(),
            entity->get_y(),
            entity->get_body_radius()
        );

        return is_touching(entity_circle);
    }

    return false;
}

void Entity::invalidate_body() {
    m_body_invalidated = true;
}

void Entity::before_simulation_update(b2WorldId world) {
    if (m_body_invalidated) {
        if (m_has_body) {
            b2DestroyBody(m_b2_body);
            m_has_body = false;
        }

        m_body_invalidated = false;
    
        if (m_body_shape != BodyShape::None) {
            b2BodyDef body_def = b2DefaultBodyDef();

            // Create a new body.
            if (m_body_type == BodyType::Static) {
                body_def.type = b2_staticBody;
            } else if (m_body_type == BodyType::Dynamic) {
                body_def.type = b2_dynamicBody;
            } else if (m_body_type == BodyType::Kinematic) {
                body_def.type = b2_kinematicBody;
            } else {
                log::error("Invalid body type: " + to_string(static_cast<int>(m_body_type)));
                return;
            }

            m_b2_body = b2CreateBody(world, &body_def);

            if (b2Body_IsValid(m_b2_body)) {
                m_has_body = true;
                b2Body_SetUserData(m_b2_body, this);
            } else {
                log::error("Failed to create body");
                return;
            }

            b2ShapeDef shape_def = b2DefaultShapeDef();

            if (m_body_shape == BodyShape::Rectangle) {
                b2Polygon rectangle = b2MakeBox(m_body_width, m_body_height);
                m_b2_shape = b2CreatePolygonShape(m_b2_body, &shape_def, &rectangle);
            } else if (m_body_shape == BodyShape::Circle) {
                b2Circle circle = {
                    b2Vec2(0.0f, 0.0f),
                    static_cast<float>(m_body_radius)
                };
                m_b2_shape = b2CreateCircleShape(m_b2_body, &shape_def, &circle);
            } else {
                log::error("Invalid body shape: " + to_string(static_cast<int>(m_body_shape)));
                return;
            }

            if (b2Shape_IsValid(m_b2_shape) == false) {
                log::error("Failed to create shape");
                return;
            }
        }
    }

    if (m_has_body) {
        // Make sure body is at the same position as the entity
        b2Body_SetTransform(m_b2_body, b2Vec2(m_position.x(), m_position.y()), b2Rot_identity);
        b2Body_SetLinearVelocity(m_b2_body, b2Vec2(m_velocity.x(), m_velocity.y()));
        b2Shape_SetFilter(m_b2_shape, m_b2_filter);
    }
}

void Entity::after_simulation_update(b2WorldId world) {
    // Update entity position based on physics body.
    if (m_has_body) {
        auto position = b2Body_GetPosition(m_b2_body);
        m_position.set(position.x, position.y);
    }
}

void Entity::set_layer(const NomadInteger layer) {
    m_layer = layer;
    invalidate_body();
}

NomadInteger Entity::get_layer() const {
    return m_layer;
}

void Entity::set_sprite_name(const NomadString& sprite_name) {
    auto sprite = m_scene->get_game()->get_resources()->get_sprites()->get_sprite_by_name(sprite_name);

    if (sprite == nullptr) {
        log::warning("Sprite '" + sprite_name + "' not found");
        return;
    }

    set_sprite(sprite);
}

const NomadString& Entity::get_sprite_name() const {
    if (m_sprite == nullptr) {
        return NOMAD_EMPTY_STRING;
    }

    return m_sprite->get_name();
}

void Entity::update(Scene* scene) {
    auto game = scene->get_game();

    if (m_on_frame != NOMAD_INVALID_ID) {
        game->execute_script_in_context(m_on_frame, &m_execution_context);
    }

    // Do we need to select new animation?
    if (m_animation_dirty) {
        m_animation_dirty = false;

        auto animation = game->get_resources()->get_animations()->get_animation(
            m_animation_name,
            m_animation_variant,
            m_animation_direction
        );

        if (animation == nullptr) {
            log::warning(
                "Animation '" + m_animation_name + "_" + m_animation_variant + "_" + m_animation_direction +
                "' not found"
            );
        }

        set_animation(animation);
    }

    // Update animation
    if (m_animation) {
        m_frame_count--;

        if (m_frame_count <= 0) {
            if (m_animation_reverse) {
                m_current_frame--;

                if (m_current_frame <= 0) {
                    if (m_animation_repeat) {
                        m_current_frame = m_animation->get_frame_count() - 1;
                    } else {
                        m_current_frame = 0;
                    }
                }
            } else {
                m_current_frame++;

                if (m_current_frame >= m_animation->get_frame_count()) {
                    if (m_animation_repeat) {
                        m_current_frame = 0;
                    } else {
                        m_current_frame = m_animation->get_frame_count() - 1;
                    }
                }
            }

            m_frame_count = m_animation_duration;

            const auto sprite = m_animation->get_frame(m_current_frame);

            set_sprite(sprite);
        }
    }
}

void Entity::render(Canvas* canvas) {
    NomadFloat entity_x = get_x();
    NomadFloat entity_y = get_y();

    if (m_sprite != nullptr) {
        auto sprite_x = entity_x - get_sprite_x();
        auto sprite_y = entity_y - get_sprite_y();

        canvas->render_sprite(m_sprite, sprite_x, sprite_y);
    }

//    if (m_text_texture == nullptr && !m_text.empty()) {
//        // Wrap text
//        auto text_width = static_cast<int>(m_text_width);
//
//        auto font = m_scene->get_game()->get_resources()->get_fonts()->get_font(m_font_id);
//        if (m_text.empty() || m_text_width <= 0) {
//            m_wrapped_text = m_text;
//        } else if (font == nullptr) {
//            log::warning("Font not set for entity '" + m_name + "'");
//            m_wrapped_text = m_text;
//        } else {
//            m_wrapped_text.clear();
//            std::string current_line;
//            std::string word;
//            std::istringstream stream(m_text);
//
//            while (stream >> word) {
//                std::string test_line = current_line += (current_line.empty() ? "" : " ") + word;
//                int line_width = font->get_text_width(test_line);
//
//                if (line_width > text_width) {
//                    if (!current_line.empty()) {
//                        m_wrapped_text += current_line + "\n";
//                        current_line = word;
//                    } else {
//                        m_wrapped_text += word + "\n";
//                        current_line.clear();
//                    }
//                } else {
//                    current_line = test_line;
//                }
//            }
//
//            if (!current_line.empty()) {
//                m_wrapped_text += current_line;
//            }
//        }
//
//        generate_text_texture(canvas);
//    }

    if (m_text_texture == nullptr && !m_text.empty()) {
        generate_text_texture(canvas);
    }

    if (m_text_texture != nullptr) {
        Point anchor;
        auto text_x = static_cast<int>(m_text_x);
        auto text_y = static_cast<int>(m_text_y);

//        int text_width = static_cast<int>(m_text_width);
//        int text_height = static_cast<int>(m_text_height);
        int text_width, text_height;
        SDL_QueryTexture(m_text_texture, nullptr, nullptr, &text_width, &text_height);

        switch (m_text_alignment) {
            case Alignment::TopLeft:
                anchor.set(text_x, text_y);
                break;

            case Alignment::TopMiddle:
                anchor.set(text_x - (text_width / 2), text_y);
                break;

            case Alignment::TopRight:
                anchor.set(text_x - text_width, text_y);
                break;

            case Alignment::CenterLeft:
                anchor.set(text_x, text_y - (text_height / 2));
                break;

            case Alignment::CenterMiddle:
                anchor.set(text_x - (text_width / 2), text_y - (text_height / 2));
                break;

            case Alignment::CenterRight:
                anchor.set(text_x - text_width, text_y  - (text_height / 2));
                break;

            case Alignment::BottomLeft:
                anchor.set(text_x, text_y - text_height);
                break;

            case Alignment::BottomMiddle:
                anchor.set(text_x - (text_width / 2), text_y - text_height);
                break;

            case Alignment::BottomRight:
                anchor.set(text_x - text_width, text_y - text_height);
                break;

            default:
                log::warning(
                    "Invalid text alignment " +
                    to_string(static_cast<int>(m_text_alignment)) +
                    " for entity '" +
                    m_name +
                    "'"
                );
        }

        auto destination = SDL_Rect{
            anchor.x() + static_cast<int>(entity_x),
            anchor.y() +  static_cast<int>(entity_y),
            text_width,
            text_height
        };

        SDL_RenderCopy(
            canvas->get_sdl_renderer(),
            m_text_texture,
            nullptr,
            &destination
        );
    }
}

NomadId Entity::get_id() const {
    return m_id;
}

void Entity::set_sprite(const Sprite* sprite) {
    if (sprite == m_sprite) {
        return;
    }

    m_sprite = sprite;
}

const Sprite* Entity::get_sprite() const {
    return m_sprite;
}

void Entity::set_sprite_x(NomadFloat x) {
    m_sprite_anchor.set_x(x);
}

NomadFloat Entity::get_sprite_x() const {
    return m_sprite_anchor.x();
}

void Entity::set_sprite_y(NomadFloat y) {
    m_sprite_anchor.set_y(y);
}

NomadFloat Entity::get_sprite_y() const {
    return m_sprite_anchor.y();
}

void Entity::set_sprite_anchor(const PointF& anchor) {
    m_sprite_anchor = anchor;
}

void Entity::set_sprite_anchor(NomadFloat x, NomadFloat y) {
    m_sprite_anchor.set(x, y);
}

const PointF& Entity::get_sprite_anchor() const {
    return m_sprite_anchor;
}

void Entity::set_animation(const Animation* animation) {
    if (animation == m_animation) {
        return;
    }

    m_animation = animation;
    m_frame_count = 0;

    if (m_animation == nullptr) {
        set_sprite(nullptr);
        return;
    }

    if (m_animation_reverse) {
        set_sprite(m_animation->get_last_frame());
    } else {
        set_sprite(m_animation->get_frame(0));
    }
}

[[nodiscard]] const Animation* Entity::get_animation() const {
    return m_animation;
}

void Entity::set_animation_name(const NomadString& animation_name) {
    if (animation_name == m_animation_name) {
        return;
    }

    m_animation_name = animation_name;
    m_animation_dirty = true;
}

const NomadString& Entity::get_animation_name() const {
    return m_animation_name;
}

void Entity::set_animation_variant(const NomadString& animation_variant) {
    if (animation_variant == m_animation_variant) {
        return;
    }

    m_animation_variant = animation_variant;
    m_animation_dirty = true;
}

const NomadString& Entity::get_animation_variant() const {
    return m_animation_variant;
}

void Entity::set_animation_direction(const NomadString& animation_direction) {
    if (animation_direction == m_animation_direction) {
        return;
    }

    m_animation_direction = animation_direction;
    m_animation_dirty = true;
}

const NomadString& Entity::get_animation_direction() const {
    return m_animation_direction;
}

void Entity::set_animation_duration(NomadInteger speed) {
    if (speed == m_animation_duration) {
        return;
    }

    m_animation_duration = speed;
}

NomadInteger Entity::get_animation_duration() const {
    return m_animation_duration;
}

void Entity::set_animation_repeat(bool repeat) {
    if (repeat == m_animation_repeat) {
        return;
    }

    m_animation_repeat = repeat;
}

bool Entity::get_animation_repeat() const {
    return m_animation_repeat;
}

void Entity::set_animation_reverse(bool reverse) {
    if (reverse == m_animation_reverse) {
        return;
    }

    m_animation_reverse = reverse;
}

bool Entity::get_animation_reverse() const {
    return m_animation_reverse;
}

void Entity::set_text(const NomadString& text) {
    if (text != m_text) {
        m_text = text;

        invalidate_text_texture();
    }
}

const NomadString& Entity::get_text() {
    return m_text;
}

void Entity::set_text_alignment(Alignment alignment) {
    m_text_alignment = alignment;
}

Alignment Entity::get_text_alignment() const {
    return m_text_alignment;
}

void Entity::set_text_x(NomadFloat x) {
    m_text_x = x;
}

NomadFloat Entity::get_text_x() const {
    return m_text_x;
}

void Entity::set_text_y(NomadFloat y) {
    m_text_y = y;
}

NomadFloat Entity::get_text_y() const {
    return m_text_y;
}

void Entity::set_text_width(NomadFloat width) {
    if (width == m_text_width) {
        return;
    }

    m_text_width = width;

    invalidate_text_texture();
}

NomadFloat Entity::get_text_width() const {
    return m_text_width;
}

void Entity::set_text_height(NomadFloat height) {
    if (height == m_text_height) {
        return;
    }

    m_text_height = height;

    invalidate_text_texture();
}

NomadFloat Entity::get_text_height() const {
    return m_text_height;
}

void Entity::set_text_line_spacing(NomadFloat line_spacing) {
    if (line_spacing == m_text_line_spacing) {
        return;
    }

    m_text_line_spacing = line_spacing;

    invalidate_text_texture();
}

NomadFloat Entity::get_text_line_spacing() const {
    return m_text_line_spacing;
}

void Entity::set_text_color(const Color& color) {
    if (color == m_text_color) {
        return;
    }
    m_text_color = color;

    invalidate_text_texture();
}

Color Entity::get_text_color() const {
    return m_text_color;
}

void Entity::set_font_by_id(NomadId font_id) {
    if (font_id == m_font_id) {
        return;
    }

    m_font_id = font_id;

    invalidate_text_texture();
}

NomadId Entity::get_font_id() const {
    return m_font_id;
}

void Entity::set_variable_value(NomadId variable_id, const ScriptValue& value) {
    m_variables.set_variable_value(variable_id, value);
}

void Entity::get_variable_value(NomadId variable_id, ScriptValue& value) const {
    m_variables.get_variable_value(variable_id, value);
}

void Entity::add_event(const NomadString& name, NomadId script_id) {
    m_events.add_event(name, script_id);
}

void Entity::remove_event(const NomadString& name) {
    m_events.remove_event(name);
}

GameExecutionContext* Entity::get_execution_context() {
    return &m_execution_context;
}

NomadId Entity::get_on_frame() const {
    return m_on_frame;
}

void Entity::set_on_frame(NomadId script_id) {
    m_on_frame = script_id;
}

NomadId Entity::get_on_collision_begin() const {
    return m_on_collision_begin;
}

void Entity::set_on_collision_begin(NomadId script_id) {
    m_on_collision_begin = script_id;
}

NomadId Entity::get_on_collision_end() const {
    return m_on_collision_end;
}

void Entity::set_on_collision_end(NomadId script_id) {
    m_on_collision_end = script_id;
}

void Entity::invalidate_text_texture() {
    if (m_text_texture) {
        SDL_DestroyTexture(m_text_texture);

        m_text_texture = nullptr;
    }
}

void Entity::generate_text_texture(Canvas* canvas) {
    if (m_text_texture) {
        SDL_DestroyTexture(m_text_texture);

        m_text_texture = nullptr;
    }

    if (m_font_id == NOMAD_INVALID_ID) {
        log::warning("No font set for entity '" + m_name + "'");
    } else {
        auto font = canvas->get_game()->get_resources()->get_fonts()->get_font(m_font_id);

        m_text_texture = font->generate_texture(
            canvas,
            m_text,
            m_text_color,
            get_horizontal_alignment(m_text_alignment),
            m_text_width,
            m_text_height,
            m_text_line_spacing
        );
    }
}

} // namespace nomad