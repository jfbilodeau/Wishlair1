//
// Created by Jean-François Bilodeau on 2023-06-17.
//

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".body.rectangle",
    [this](Interpreter* interpreter) {
        auto body_type = static_cast<BodyType>(interpreter->get_integer_parameter(0));
        auto width = interpreter->get_float_parameter(1);
        auto height = interpreter->get_float_parameter(2);

        START_ENTITY_BLOCK("Cannot set rectangle body on a null entity")

        entity->set_rectangle_body(body_type, width, height);

        END_ENTITY_BLOCK
    }, {
        def_parameter("bodyType", m_runtime->get_integer_type(), NomadParamDoc("Body type.")),
        def_parameter("width", m_runtime->get_float_type(), NomadParamDoc("Width of the rectangle body.")),
        def_parameter("height", m_runtime->get_float_type(), NomadParamDoc("Height of the rectangle body.")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the rectangle body of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".body.circle",
    [this](Interpreter* interpreter) {
        auto body_type = static_cast<BodyType>(interpreter->get_integer_parameter(0));
        auto radius = interpreter->get_float_parameter(1);

        START_ENTITY_BLOCK("Cannot set circle body on a null entity")

        entity->set_circle_body(body_type, radius);

        END_ENTITY_BLOCK
    }, {
        def_parameter("bodyType", m_runtime->get_integer_type(), NomadParamDoc("Body type.")),
        def_parameter("radius", m_runtime->get_float_type(), NomadParamDoc("Radius of the circle body.")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the circle body of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".body.none",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot set no body on a null entity")

        entity->set_no_body();

        END_ENTITY_BLOCK
    },
    {},
    m_runtime->get_void_type(),
    NomadDoc("Set no body on the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".moveTo",
    [this](Interpreter* interperter) {
        auto x = interperter->get_float_parameter(0);
        auto y = interperter->get_float_parameter(1);
        auto speed = interperter->get_float_parameter(2);
        auto script_id = interperter->get_id_parameter(3);

        START_ENTITY_BLOCK("Cannot call this.moveTo to events on a null entity")

        entity->move_to(x, y, speed, script_id);

        END_ENTITY_BLOCK
    },
    {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate of the destination.")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate of the destination.")),
        def_parameter("speed", m_runtime->get_float_type(), NomadParamDoc("Speed to move to the destination.")),
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Move the entity to a specified location.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".on",
    [this](Interpreter* interpreter) {
        NomadString event_name = interpreter->get_parameter(0).get_string_value();
        auto script_id = interpreter->get_parameter(1).get_id_value();

        auto scene = get_current_context()->get_scene();

        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        if (script_id == NOMAD_INVALID_ID) {
            log::error("Script or function for event '" + event_name + "' not found");
            return;
        }

        //entity->add_event(event_name, script_id);
        scene->register_entity_event(event_name, entity->get_id(), script_id);

        END_ENTITY_BLOCK
    }, {
        def_parameter("eventName", m_runtime->get_string_type(), NomadParamDoc("Name of the event to listen to.")),
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script or function to an event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".on.frame",
    [this](Interpreter* interpreter) {
        auto script_id = interpreter->get_id_parameter(0);

        START_ENTITY_BLOCK("Cannot set on.frame on a null entity")

        if (script_id == NOMAD_INVALID_ID) {
            log::error("Script for frame event not found");
            return;
        }
        
        entity->set_on_frame(script_id);

        END_ENTITY_BLOCK
    }, {
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script to the frame event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".on.collisionStart",
    [this](Interpreter* interpreter) {
        auto script_id = interpreter->get_id_parameter(0);

        START_ENTITY_BLOCK("Cannot set on.collisionStart on a null entity")

        if (script_id == NOMAD_INVALID_ID) {
            log::error("Script for collision start event not found");
            return;
        }

        entity->set_on_collision_begin(script_id);

        END_ENTITY_BLOCK
    }, {
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script to the collision start event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".on.collisionEnd",
    [this](Interpreter* interpreter) {
        auto script_id = interpreter->get_id_parameter(0);

        START_ENTITY_BLOCK("Cannot set on.collisionEnd on a null entity")

        if (script_id == NOMAD_INVALID_ID) {
            log::error("Script or function for collision end event not found");
            return;
        }

        entity->set_on_collision_end(script_id);

        END_ENTITY_BLOCK
    }, {
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script or function to the collision end event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".setLocation",
    [this](Interpreter* interpreter) {
        auto x = interpreter->get_float_parameter(0);
        auto y = interpreter->get_float_parameter(1);

        START_ENTITY_BLOCK("Cannot set location on a null entity")

        entity->set_location(x, y);

        END_ENTITY_BLOCK
    }, {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the location of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".trigger",
    [this](Interpreter* interpreter) {
        auto event_name = interpreter->get_string_parameter(0);

        auto scene = get_current_context()->get_scene();

        START_ENTITY_BLOCK("Cannot trigger event on a null entity")

        scene->trigger_event(event_name, entity);

        END_ENTITY_BLOCK
    }, {
        def_parameter("eventName", m_runtime->get_string_type(), NomadParamDoc("Name of the event to trigger.")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Trigger an event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".velocity.set",
    [this](Interpreter* interpreter) {
        auto x = interpreter->get_float_parameter(0);
        auto y = interpreter->get_float_parameter(1);

        START_ENTITY_BLOCK("Cannot set velocity on a null entity")

        entity->set_velocity(x, y);

        END_ENTITY_BLOCK
    }, {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X velocity")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y velocity")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the velocity of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".velocity.clear",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot clear velocity on a null entity")

        entity->set_velocity(0, 0);

        END_ENTITY_BLOCK
    },
    {},
    m_runtime->get_void_type(),
    NomadDoc("Clear the velocity of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".getCollidingMask",
    [this](Interpreter* interpreter) {
        START_SINGLE_ENTITY_BLOCK("Cannot get collision mask of a null entity")

        auto scene = entity->get_scene();

        auto result = scene->get_mask_at_entity(entity);

        interpreter->set_integer_result(result);

        END_SINGLE_ENTITY_BLOCK(0)
    },
    {},
    m_runtime->get_integer_type(),
    NomadDoc("Get the collision mask at the entity's position.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".getCollidingMaskAt",
    [this](Interpreter* interpreter) {
        auto x = interpreter->get_float_parameter(0);
        auto y = interpreter->get_float_parameter(1);

        START_SINGLE_ENTITY_BLOCK("Cannot get collision mask of a null entity")

        auto scene = entity->get_scene();

        auto result = scene->get_mask_at_entity(entity, {x, y});

        interpreter->set_integer_result(result);

        END_SINGLE_ENTITY_BLOCK(0)
    },
    {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->get_integer_type(),
    NomadDoc("Get the collision mask for the entity body at the specified position.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".setSprite",
    [this](Interpreter* interpreter) {
        auto sprite_name = interpreter->get_string_parameter(0);

        START_ENTITY_BLOCK("Cannot set sprite on a null entity")

        auto sprite = m_resource_manager->get_sprites()->get_sprite_by_name(sprite_name);

        entity->set_sprite(sprite);

        END_ENTITY_BLOCK
    }, {
        def_parameter("name", m_runtime->get_string_type(), NomadParamDoc("Name of the sprite to set.")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the sprite of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".setSpriteAnchor",
    [this](Interpreter* interpreter) {
        auto x = interpreter->get_float_parameter(0);
        auto y = interpreter->get_float_parameter(1);

        START_ENTITY_BLOCK("Cannot set sprite anchor on a null entity")

        entity->set_location(entity->get_x() - x, entity->get_y() - y);
        entity->set_sprite_anchor(x, y);

        END_ENTITY_BLOCK
    }, {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the anchor point of the sprite of the entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".repositionOnAnchor",
    [this](Interpreter* interpreter) {
        auto anchor_x = interpreter->get_float_parameter(0);
        auto anchor_y = interpreter->get_float_parameter(1);

        START_ENTITY_BLOCK("Cannot reposition on anchor on a null entity")

        auto new_x = entity->get_x() + anchor_x;
        auto new_y = entity->get_y() + anchor_y;

        entity->set_location(new_x, new_y);
        // entity->set_text_position(anchor_x, anchor_y);
        entity->set_sprite_anchor(anchor_x, anchor_y);

        END_ENTITY_BLOCK
    },
    {
        def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X coordinate")),
        def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Reposition the entity based on the new sprite anchor.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".setAnimation",
    [this](Interpreter* interpreter) {
        auto animation_name = interpreter->get_string_parameter(0);
        auto animation_variant = interpreter->get_string_parameter(1);
        auto animation_direction = interpreter->get_string_parameter(2);
        auto duration = interpreter->get_integer_parameter(3);
        auto repeat = interpreter->get_boolean_parameter(4);
        auto reverse = interpreter->get_boolean_parameter(5);

        START_ENTITY_BLOCK("Cannot set animation on a null entity")

        entity->set_animation_duration(duration);
        entity->set_animation_repeat(repeat);
        entity->set_animation_reverse(reverse);

        auto animation = m_resource_manager->get_animations()->get_animation(
            animation_name,
            animation_variant,
            animation_direction
        );

        entity->set_animation(animation);

        END_ENTITY_BLOCK
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
    VARIABLE_NAME_PREFIX ".setText",
    [this](Interpreter* interpreter) {
        auto font = interpreter->get_id_parameter(0);
        auto text = interpreter->get_string_parameter(1);

        START_ENTITY_BLOCK("Cannot set text on a null entity")

        entity->set_font_by_id(font);
        entity->set_text(text);

        END_ENTITY_BLOCK
    }, {
        def_parameter("fontId", m_runtime->get_integer_type(), NomadParamDoc("ID of the font to use")),
        def_parameter("text", m_runtime->get_string_type(), NomadParamDoc("Text to set")),
    },
    m_runtime->get_void_type(),
    NomadDoc("Set the font and text to display on an entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".onPress",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        const auto action_name = interpreter->get_string_parameter(0);
        const auto script_id = interpreter->get_id_parameter(1);
        const auto entity_id = entity->get_id();

        get_current_context()->get_scene()->add_action_pressed(
            action_name,
            script_id,
            entity_id
        );

        END_ENTITY_BLOCK
    }, {
        def_parameter("actionName", m_runtime->get_string_type(), NomadParamDoc("Name of the action to listen to.")),
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script"))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script or function to the pressed event for this entity.")
);

m_runtime->register_command(
    VARIABLE_NAME_PREFIX ".onRelease",
    [this](Interpreter* interpreter) {
        auto action_name = interpreter->get_string_parameter(0);
        auto script_id = interpreter->get_id_parameter(1);

        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        get_current_context()->get_scene()->add_action_released(
            action_name,
            script_id,
            entity->get_id()
        );

        END_ENTITY_BLOCK
    }, {
        def_parameter("actionName", m_runtime->get_string_type(), NomadParamDoc("Name of the action to listen to.")),
        def_parameter("callback", m_runtime->get_callback_type({}, m_runtime->get_void_type()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->get_void_type(),
    NomadDoc("Assign a script or function to the release event for this entity.")
);
