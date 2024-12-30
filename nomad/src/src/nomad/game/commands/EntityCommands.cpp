//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/Animation.hpp"
#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Sprite.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

#define CHECK_ENTITY_NOT_NULL(message) \
    if (get_current_context()->get_this_entity() == nullptr) { \
        log::error(message);  \
        return; \
    }

namespace nomad {

void Game::init_entity_commands() {
    log::debug("Initializing entity commands");

    m_runtime->register_command(
        "this.on",
        [this](Interpreter* interpreter) {
            NomadString event_name = interpreter->get_parameter(0).get_string_value();
            auto script_id = interpreter->get_parameter(1).get_id_value();

            CHECK_ENTITY_NOT_NULL("Cannot listen to events on a null entity")

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script or function for event '" + event_name + "' not found");
                return;
            }

            auto entity = get_current_context()->get_this_entity();
            entity->add_event(event_name, script_id);
        }, {
            def_parameter("eventName", m_runtime->get_string_type(), NomadParamDoc("Name of the event to listen to.")),
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to an event for this entity.")
    );

    m_runtime->register_command(
        "this.onFrame",
        [this](Interpreter* interpreter) {
            auto script_id = interpreter->get_id_parameter(0);

            CHECK_ENTITY_NOT_NULL("Cannot set onFrame on a null entity")

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script or function for frame event not found");
                return;
            }

            auto entity = get_current_context()->get_this_entity();

            entity->set_on_frame(script_id);
        }, {
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to the frame event for this entity.")
    );

    m_runtime->register_command(
        "this.onCollisionStart",
        [this](Interpreter* interpreter) {
            auto script_id = interpreter->get_id_parameter(0);

            CHECK_ENTITY_NOT_NULL("Cannot set onCollisionStart on a null entity")

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script or function for collision start event not found");
                return;
            }

            auto entity = get_current_context()->get_this_entity();

            entity->set_on_collision_begin(script_id);
        }, {
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to the collision start event for this entity.")
    );

    m_runtime->register_command(
        "this.onCollisionEnd",
        [this](Interpreter* interpreter) {
            auto script_id = interpreter->get_id_parameter(0);

            CHECK_ENTITY_NOT_NULL("Cannot set onCollisionEnd on a null entity")

            if (script_id == NOMAD_INVALID_ID) {
                log::error("Script or function for collision end event not found");
                return;
            }

            auto entity = get_current_context()->get_this_entity();

            entity->set_on_collision_end(script_id);
        }, {
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to the collision end event for this entity.")
    );

    m_runtime->register_command(
            "this.setLocation",
            [this](Interpreter* interpreter) {
                auto x = interpreter->get_float_parameter(0);
                auto y = interpreter->get_float_parameter(1);

                CHECK_ENTITY_NOT_NULL("Cannot set location on a null entity")

                get_current_context()->get_this_entity()->set_location(x, y);
            }, {
                def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
                def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
            },
            m_runtime->get_void_type(),
            NomadDoc("Set the location of the entity.")
    );

    m_runtime->register_command(
        "this.body.rectangle",
        [this](Interpreter* interpreter) {
            auto body_type = static_cast<BodyType>(interpreter->get_integer_parameter(0));
            auto width = interpreter->get_float_parameter(1);
            auto height = interpreter->get_float_parameter(2);

            CHECK_ENTITY_NOT_NULL("Cannot set rectangle body on a null entity")

            get_current_context()->get_this_entity()->set_rectangle_body(body_type, width, height);
        }, {
            def_parameter("bodyType", m_runtime->get_integer_type(), NomadParamDoc("Body type.")),
            def_parameter("width", m_runtime->get_float_type(), NomadParamDoc("Width of the rectangle body.")),
            def_parameter("height", m_runtime->get_float_type(), NomadParamDoc("Height of the rectangle body.")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the rectangle body of the entity.")
    );

    m_runtime->register_command(
        "this.body.circle",
        [this](Interpreter* interpreter) {
            auto body_type = static_cast<BodyType>(interpreter->get_integer_parameter(0));
            auto radius = interpreter->get_float_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot set circle body on a null entity")

            get_current_context()->get_this_entity()->set_circle_body(body_type, radius);
        }, {
            def_parameter("bodyType", m_runtime->get_integer_type(), NomadParamDoc("Body type.")),
            def_parameter("radius", m_runtime->get_float_type(), NomadParamDoc("Radius of the circle body.")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the circle body of the entity.")
    );

    m_runtime->register_command(
        "this.body.none",
        [this](Interpreter* interpreter) {
            CHECK_ENTITY_NOT_NULL("Cannot set no body on a null entity")

            get_current_context()->get_this_entity()->set_no_body();
        },
        {},
        m_runtime->get_void_type(),
        NomadDoc("Set no body on the entity.")
    );

    m_runtime->register_command(
        "this.velocity.set",
        [this](Interpreter* interpreter) {
            auto x = interpreter->get_float_parameter(0);
            auto y = interpreter->get_float_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot set velocity on a null entity")

            get_current_context()->get_this_entity()->set_velocity(x, y);
        }, {
            def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X velocity")),
            def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y velocity")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the velocity of the entity.")
    );

    m_runtime->register_command(
        "this.velocity.clear",
        [this](Interpreter* interpreter) {
            CHECK_ENTITY_NOT_NULL("Cannot clear velocity on a null entity")

            get_current_context()->get_this_entity()->set_velocity(0, 0);
        },
        {},
        m_runtime->get_void_type(),
        NomadDoc("Clear the velocity of the entity.")
    );

    m_runtime->register_command(
        "this.getCollidingMask",
        [this](Interpreter* interpreter) {
            CHECK_ENTITY_NOT_NULL("Cannot get collision mask of a null entity")

            auto entity = get_current_context()->get_this_entity();

            auto scene = entity->get_scene();

            auto result = scene->get_mask_at_entity(entity);

            interpreter->set_integer_result(result);
        },
        {},
        m_runtime->get_integer_type(),
        NomadDoc("Get the collision mask at the entity's position.")
    );

    m_runtime->register_command(
        "this.getCollidingMaskAt",
        [this](Interpreter* interpreter) {
            auto x = interpreter->get_float_parameter(0);
            auto y = interpreter->get_float_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot get collision mask of a null entity")

            auto entity = get_current_context()->get_this_entity();

            auto scene = entity->get_scene();

            auto result = scene->get_mask_at_entity(entity, {x, y});

            interpreter->set_integer_result(result);
        },
        {
            def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
            def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Get the collision mask for the entity body at the specified position.")
    );

    m_runtime->register_command(
        "this.setSprite",
        [this](Interpreter* interpreter) {
            auto sprite_name = interpreter->get_string_parameter(0);

            CHECK_ENTITY_NOT_NULL("Cannot set sprite on a null entity")

            auto sprite = m_resource_manager->get_sprites()->get_sprite_by_name(sprite_name);

            auto entity = get_current_context()->get_this_entity();

            entity->set_sprite(sprite);
        }, {
            def_parameter("name", m_runtime->get_string_type(), NomadParamDoc("Name of the sprite to set.")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the sprite of the entity.")
    );

    m_runtime->register_command(
        "this.setSpriteAnchor",
        [this](Interpreter* interpreter) {
            auto x = interpreter->get_float_parameter(0);
            auto y = interpreter->get_float_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot set sprite anchor on a null entity")

            auto entity = get_current_context()->get_this_entity();
            entity->set_location(entity->get_x() - x, entity->get_y() - y);
            entity->set_sprite_anchor(x, y);
        }, {
            def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
            def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the anchor point of the sprite of the entity.")
    );

    m_runtime->register_command(
        "this.repositionOnAnchor",
        [this](Interpreter* interpreter) {
            auto anchor_x = interpreter->get_float_parameter(0);
            auto anchor_y = interpreter->get_float_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot reposition on anchor on a null entity")

            auto entity = get_current_context()->get_this_entity();

            auto new_x = entity->get_x() + anchor_x;
            auto new_y = entity->get_y() + anchor_y;

            entity->set_location(new_x, new_y);
            entity->set_sprite_anchor(anchor_x, anchor_y);
        },
        {
            def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
            def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Reposition the entity based on the new sprite anchor.")
    );

    m_runtime->register_command(
        "this.setAnimation",
        [this](Interpreter* interpreter) {
            auto animation_name = interpreter->get_string_parameter(0);
            auto animation_variant = interpreter->get_string_parameter(1);
            auto animation_direction = interpreter->get_string_parameter(2);
            auto duration = interpreter->get_integer_parameter(3);
            auto repeat = interpreter->get_boolean_parameter(4);
            auto reverse = interpreter->get_boolean_parameter(5);

            CHECK_ENTITY_NOT_NULL("Cannot set animation on a null entity")

            auto entity = get_current_context()->get_this_entity();

            entity->set_animation_duration(duration);
            entity->set_animation_repeat(repeat);
            entity->set_animation_reverse(reverse);

            auto animation = m_resource_manager->get_animations()->get_animation(
                animation_name,
                animation_variant,
                animation_direction
            );

            entity->set_animation(animation);
        }, {
            def_parameter("name", m_runtime->get_string_type(), NomadParamDoc("Name of the animation to set.")),
            def_parameter("variant", m_runtime->get_string_type(), NomadParamDoc("Variant name of the animation to set.")),
            def_parameter("direction", m_runtime->get_string_type(), NomadParamDoc("Direction name of the animation to set.")),
            def_parameter("duration", m_runtime->get_integer_type(), NomadParamDoc("Number of frames to display an animation frame.")),
            def_parameter("repeat", m_runtime->get_boolean_type(), NomadParamDoc("Whether to repeat the animation.")),
            def_parameter("reverse", m_runtime->get_boolean_type(), NomadParamDoc("Whether to reverse the animation.")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the animation of the entity.")
    );

    m_runtime->register_command(
        "this.setText",
        [this](Interpreter* interpreter) {
            auto font = interpreter->get_id_parameter(0);
            auto text = interpreter->get_string_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot set text on a null entity")

            get_current_context()->get_this_entity()->set_font_by_id(font);
            get_current_context()->get_this_entity()->set_text(text);
        }, {
            def_parameter("fontId", m_runtime->get_integer_type(), NomadParamDoc("ID of the font to use")),
            def_parameter("text", m_runtime->get_string_type(), NomadParamDoc("Text to set")),
        },
        m_runtime->get_void_type(),
        NomadDoc("Set the font and text to display on an entity.")
    );

    m_runtime->register_command(
        "this.onPress",
        [this](Interpreter* interpreter) {
            CHECK_ENTITY_NOT_NULL("Cannot listen to events on a null entity")

            const auto action_name = interpreter->get_string_parameter(0);
            const auto script_id = interpreter->get_id_parameter(1);
            const auto entity_id = get_current_context()->get_this_entity()->get_id();

            get_current_context()->get_scene()->add_action_pressed(
                action_name,
                script_id,
                entity_id
            );
        }, {
            def_parameter("actionName", m_runtime->get_string_type(), NomadParamDoc("Name of the action to listen to.")),
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script"))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to the pressed event for this entity.")
    );

    m_runtime->register_command(
        "this.onRelease",
        [this](Interpreter* interpreter) {
            auto action_name = interpreter->get_string_parameter(0);
            auto script_id = interpreter->get_id_parameter(1);

            CHECK_ENTITY_NOT_NULL("Cannot listen to events on a null entity")

            get_current_context()->get_scene()->add_action_released(
                action_name,
                script_id,
                get_current_context()->get_this_entity()->get_id()
            );
        }, {
            def_parameter("actionName", m_runtime->get_string_type(), NomadParamDoc("Name of the action to listen to.")),
            def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Assign a script or function to the release event for this entity.")
    );
}

} // nomad