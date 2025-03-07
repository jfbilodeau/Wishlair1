m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".body.height",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body height of null entity")

        value.set_float_value(entity->get_body_height());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The body height of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".body.shape",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body shape of null entity")

        value.set_integer_value(static_cast<NomadInteger>(entity->get_body_shape()));

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The body shape of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".body.width",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body width of null entity")

        value.set_float_value(entity->get_body_width());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The body width of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".collisionMask",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set collision mask of null entity")

        auto mask = value.get_integer_value();
        entity->set_collision_mask(mask);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get collision mask of null entity")

        value.set_integer_value(entity->get_collision_mask());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The collision mask of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".font",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set font of null entity")

        auto font_id = value.get_id_value();
        entity->set_font_by_id(font_id);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get font of null entity")

        value.set_id_value(entity->get_font_id());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The font of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".layer",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set layer position of null entity")

        auto layer = value.get_integer_value();
        entity->set_layer(layer);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get layer of null entity")

        value.set_integer_value(entity->get_layer());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The layer position of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".mask",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set mask of null entity")

        auto mask = value.get_integer_value();
        entity->set_mask(mask);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get mask of null entity")

        value.set_integer_value(entity->get_mask());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The mask of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".radius",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body radius of null entity")

        value.set_float_value(entity->get_body_radius());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The body radius of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sensor",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sensor of null entity")

        auto sensor = value.get_boolean_value();
        entity->set_sensor(sensor);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get sensor of null entity")

        value.set_boolean_value(entity->is_sensor());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->get_boolean_type(),
    NomadDoc("Determine if the body of the entity is a sensor. Default is false.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".visible",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set visible of null entity")

        auto visible = value.get_boolean_value();
        entity->set_visible(visible);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get visible of null entity")

        value.set_boolean_value(entity->is_visible());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->get_boolean_type(),
    NomadDoc("Determine if the entity is visible. Default is true.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set the name of a null entity")

        auto name = value.get_string_value();
        entity->set_name(name);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get the name of a null entity")

        value.set_string_value(entity->get_name());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->get_string_type(),
    NomadDoc("The name of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".velocity.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set velocity.x of null entity")

        auto float_value = value.get_float_value();
        entity->set_velocity_x(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get velocity.x of null entity")

        value.set_float_value(entity->get_velocity().x());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The x velocity of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".velocity.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set velocity.y of null entity")

        auto float_value = value.get_float_value();
        entity->set_velocity_y(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get velocity.y of null entity")

        value.set_float_value(entity->get_velocity().y());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The y velocity of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sprite.name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.name of null entity")

        auto sprite_name = value.get_string_value();
        entity->set_sprite_name(sprite_name);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.name of null entity")

        value.set_string_value(entity->get_sprite_name());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_string_type(),
    NomadDoc("The name of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sprite.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.x position of null entity")

        auto float_value = value.get_float_value();
        entity->set_sprite_x(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.x of null entity")

        value.set_float_value(entity->get_sprite_x());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("Read the x anchor (offset) of the sprite of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sprite.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.y position of null entity")

        auto float_value = value.get_float_value();
        entity->set_sprite_y(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.y of null entity")

        value.set_float_value(entity->get_sprite_y());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("Read the y anchor (offset) of the sprite of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sprite.width",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read width of null entity")

        auto sprite = entity->get_sprite();

        if (sprite) {
            value.set_integer_value(sprite->get_width());
        } else {
            log::warning("Attempted to read width of null sprite");
            value.set_float_value(0.0);
        }

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_integer_type(),
    NomadDoc("Read the width of the sprite of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".sprite.height",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read height of null entity")

        auto sprite = entity->get_sprite();

        if (sprite) {
            value.set_integer_value(sprite->get_height());
        } else {
            log::warning("Attempted to read sprite height of null entity");
            value.set_float_value(0.0);
        }

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_integer_type(),
    NomadDoc("Read the height of the sprite of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text of null entity")

        auto text = value.get_string_value();

        entity->set_text(text);

        END_ENTITY_BLOCK
    },
    nullptr,
    m_runtime->get_string_type(),
    NomadDoc("Set the text of the sprite of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text x position of null entity")

        entity->set_text_x(value.get_float_value());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text x position of null entity")

        value.set_float_value(entity->get_text_x());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The x position of the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text y position of null entity")

        entity->set_text_y(value.get_float_value());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text y position of null entity")

        value.set_float_value(entity->get_text_y());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The y position of the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.width",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text width position of null entity")

        entity->set_text_width(value.get_float_value());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text width position of null entity")

        value.set_float_value(entity->get_text_width());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The width of the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.height",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text height of null entity")

        entity->set_text_height(value.get_float_value());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text height of null entity")

        value.set_float_value(entity->get_text_height());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The maximum height of the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.lineSpacing",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text line spacing of null entity")

        entity->set_text_line_spacing(value.get_float_value());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text line spacing of null entity")

        value.set_float_value(entity->get_text_line_spacing());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The text line spacing of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.color",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text color of null entity")

        auto rgba = Rgba(value.get_integer_value());

        entity->set_text_color(Color(rgba));

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text color of null entity")

        auto color = entity->get_text_color();

        value.set_integer_value(color.rgba);

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The color for the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".text.alignment",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text alignment of null entity")

        auto alignment = static_cast<Alignment>(value.get_integer_value());

        entity->set_text_alignment(alignment);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text alignment of null entity")

        value.set_integer_value(static_cast<int>(entity->get_text_alignment()));

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The alignment of the text of an entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation name of null entity")

        auto animation_name = value.get_string_value();

        entity->set_animation_name(animation_name);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation name of null entity")

        value.set_string_value(entity->get_animation_name());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->get_string_type(),
    NomadDoc("The animation name of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.variant",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation variant of null entity")

        auto animation_variant = value.get_string_value();

        entity->set_animation_variant(animation_variant);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation variant of null entity")

        value.set_string_value(entity->get_animation_variant());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->get_string_type(),
    NomadDoc("The animation variant of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.direction",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation direction of null entity")

        auto animation_direction = value.get_string_value();

        entity->set_animation_direction(animation_direction);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation direction of null entity")

        value.set_string_value(entity->get_animation_direction());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->get_string_type(),
    NomadDoc("The animation direction of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.speed",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation speed of null entity")

        auto animation_speed = value.get_integer_value();

        entity->set_animation_duration(animation_speed);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation speed of null entity")

        value.set_integer_value(entity->get_animation_duration());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->get_integer_type(),
    NomadDoc("The animation speed of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.repeat",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation repeat of null entity")

        auto animation_repeat = value.get_boolean_value();

        entity->set_animation_repeat(animation_repeat);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation repeat of null entity")

        value.set_integer_value(entity->get_animation_repeat());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->get_boolean_type(),
    NomadDoc("The animation speed of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".animation.reverse",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation reverse of null entity")

        auto animation_reverse = value.get_boolean_value();

        entity->set_animation_reverse(animation_reverse);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation reverse of null entity")

        value.set_integer_value(entity->get_animation_reverse());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->get_boolean_type(),
    NomadDoc("Should the animation be played in reversed order.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Cannot set x position on a null entity")

        auto float_value = value.get_float_value();

        entity->set_x(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get x position of null entity")

        value.set_float_value(entity->get_x());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The x position of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set y position of null entity")

        auto float_value = value.get_float_value();
        entity->set_y(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get y position of null entity")

        value.set_float_value(entity->get_y());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The y position of the entity.")
);

m_runtime->register_dynamic_variable(
    VARIABLE_NAME_PREFIX ".z",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set z position of null entity")

        auto float_value = value.get_float_value();
        entity->set_z(float_value);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get z position of null entity")

        value.set_float_value(entity->get_z());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->get_float_type(),
    NomadDoc("The z position (draw order) of the entity.")
);

