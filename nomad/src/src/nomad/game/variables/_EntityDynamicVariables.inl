m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".body.height",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body height of null entity")

        value.setFloatValue(entity->getBodyHeight());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The body height of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".body.shape",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body shape of null entity")

        value.setIntegerValue(static_cast<NomadInteger>(entity->getBodyShape()));

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The body shape of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".body.width",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body width of null entity")

        value.setFloatValue(entity->getBodyWidth());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The body width of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".collisionMask",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set collision mask of null entity")

        auto mask = value.getIntegerValue();
        entity->setCollisionMask(mask);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get collision mask of null entity")

        value.setIntegerValue(entity->getCollisionMask());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The collision mask of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".font",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set font of null entity")

        auto fontId = value.getIdValue();
        entity->setFontById(fontId);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get font of null entity")

        value.setIdValue(entity->getFontId());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The font of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".height",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set height of null entity")

        auto height = value.getFloatValue();
        entity->setHeight(height);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get height of null entity")

        value.setFloatValue(entity->getHeight());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The height of an entity")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".layer",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set layer position of null entity")

        auto layer = value.getIntegerValue();
        entity->setLayer(layer);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get layer of null entity")

        value.setIntegerValue(entity->getLayer());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The layer position of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".mask",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set mask of null entity")

        auto mask = value.getIntegerValue();
        entity->setMask(mask);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get mask of null entity")

        value.setIntegerValue(entity->getMask());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The mask of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".radius",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get body radius of null entity")

        value.setFloatValue(entity->getBodyRadius());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The body radius of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sensor",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sensor of null entity")

        auto sensor = value.getBooleanValue();
        entity->setSensor(sensor);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get sensor of null entity")

        value.setBooleanValue(entity->isSensor());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->getBooleanType(),
    NomadDoc("Determine if the body of the entity is a sensor. Default is false.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".visible",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set visible of null entity")

        auto visible = value.getBooleanValue();
        entity->setVisible(visible);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get visible of null entity")

        value.setBooleanValue(entity->isVisible());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->getBooleanType(),
    NomadDoc("Determine if the entity is visible. Default is true.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".width",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set the width of a null entity")

        auto width = value.getFloatValue();

        entity->setWidth(width);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get the width of a null entity")

        value.setFloatValue(entity->getWidth());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The width of an entity")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set the name of a null entity")

        auto name = value.getStringValue();
        entity->setName(name);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get the name of a null entity")

        value.setStringValue(entity->getName());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->getStringType(),
    NomadDoc("The name of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".velocity.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set velocity.x of null entity")

        auto floatValue = value.getFloatValue();
        entity->setVelocityX(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get velocity.x of null entity")

        value.setFloatValue(entity->getVelocity().getX());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The x velocity of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".velocity.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set velocity.y of null entity")

        auto floatValue = value.getFloatValue();
        entity->setVelocityY(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get velocity.y of null entity")

        value.setFloatValue(entity->getVelocity().getY());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The y velocity of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sprite.name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.name of null entity")

        auto spriteName = value.getStringValue();
        entity->setSpriteName(spriteName);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.name of null entity")

        value.setStringValue(entity->getSpriteName());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getStringType(),
    NomadDoc("The name of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sprite.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.x position of null entity")

        auto floatValue = value.getFloatValue();
        entity->setSpriteX(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.x of null entity")

        value.setFloatValue(entity->getSpriteX());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("Read the x anchor (offset) of the sprite of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sprite.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set sprite.y position of null entity")

        auto floatValue = value.getFloatValue();
        entity->setSpriteY(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read sprite.y of null entity")

        value.setFloatValue(entity->getSpriteY());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("Read the y anchor (offset) of the sprite of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sprite.width",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read width of null entity")

        auto sprite = entity->getSprite();

        if (sprite) {
            value.setIntegerValue(sprite->getWidth());
        } else {
            log::warning("Attempted to read width of null sprite");
            value.setFloatValue(0.0);
        }

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getIntegerType(),
    NomadDoc("Read the width of the sprite of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".sprite.height",
    nullptr,
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to read height of null entity")

        auto sprite = entity->getSprite();

        if (sprite) {
            value.setIntegerValue(sprite->getHeight());
        } else {
            log::warning("Attempted to read sprite height of null entity");
            value.setFloatValue(0.0);
        }

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getIntegerType(),
    NomadDoc("Read the height of the sprite of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text of null entity")

        auto text = value.getStringValue();

        entity->setText(text);

        END_ENTITY_BLOCK
    },
    nullptr,
    m_runtime->getStringType(),
    NomadDoc("Set the text of the sprite of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text x position of null entity")

        entity->setTextX(value.getFloatValue());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text x position of null entity")

        value.setFloatValue(entity->getTextX());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The x position of the text of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text y position of null entity")

        entity->setTextY(value.getFloatValue());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text y position of null entity")

        value.setFloatValue(entity->getTextY());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The y position of the text of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.width",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text width position of null entity")

        entity->setTextWidth(value.getFloatValue());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text width position of null entity")

        value.setFloatValue(entity->getTextWidth());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The width of the text of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.height",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text height of null entity")

        entity->setTextHeight(value.getFloatValue());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text height of null entity")

        value.setFloatValue(entity->getTextHeight());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The maximum height of the text of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.lineSpacing",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text line spacing of null entity")

        entity->setTextLineSpacing(value.getFloatValue());

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text line spacing of null entity")

        value.setFloatValue(entity->getTextLineSpacing());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The text line spacing of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.color",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text color of null entity")

        auto rgba = Rgba(value.getIntegerValue());

        entity->setTextColor(Color(rgba));

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text color of null entity")

        auto color = entity->getTextColor();

        value.setIntegerValue(color.rgba);

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The text color of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".text.alignment",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set text alignment of null entity")

        auto alignment = static_cast<Alignment>(value.getIntegerValue());

        entity->setTextAlignment(alignment);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get text alignment of null entity")

        value.setIntegerValue(static_cast<int>(entity->getTextAlignment()));

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The alignment of the text of an entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.name",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation name of null entity")

        auto animationName = value.getStringValue();

        entity->setAnimationName(animationName);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation name of null entity")

        value.setStringValue(entity->getAnimationName());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->getStringType(),
    NomadDoc("The animation name of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.variant",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation variant of null entity")

        auto animationVariant = value.getStringValue();

        entity->setAnimationVariant(animationVariant);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation variant of null entity")

        value.setStringValue(entity->getAnimationVariant());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->getStringType(),
    NomadDoc("The animation variant of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.direction",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation direction of null entity")

        auto animationDirection = value.getStringValue();

        entity->setAnimationDirection(animationDirection);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation direction of null entity")

        value.setStringValue(entity->getAnimationDirection());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_STRING)
    },
    m_runtime->getStringType(),
    NomadDoc("The animation direction of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.speed",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation speed of null entity")

        auto animationSpeed = value.getIntegerValue();

        entity->setAnimationDuration(animationSpeed);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation speed of null entity")

        value.setIntegerValue(entity->getAnimationDuration());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_INTEGER)
    },
    m_runtime->getIntegerType(),
    NomadDoc("The animation speed of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.repeat",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation repeat of null entity")

        auto animationRepeat = value.getBooleanValue();

        entity->setAnimationRepeat(animationRepeat);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation repeat of null entity")

        value.setIntegerValue(entity->getAnimationRepeat());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->getBooleanType(),
    NomadDoc("The animation speed of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".animation.reverse",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set animation reverse of null entity")

        auto animationReverse = value.getBooleanValue();

        entity->setAnimationReverse(animationReverse);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get animation reverse of null entity")

        value.setIntegerValue(entity->getAnimationReverse());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_BOOLEAN)
    },
    m_runtime->getBooleanType(),
    NomadDoc("Should the animation be played in reversed order.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".x",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Cannot set x position on a null entity")

        auto floatValue = value.getFloatValue();

        entity->setX(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get x position of null entity")

        value.setFloatValue(entity->getX());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The x position of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".y",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set y position of null entity")

        auto floatValue = value.getFloatValue();
        entity->setY(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get y position of null entity")

        value.setFloatValue(entity->getY());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The y position of the entity.")
);

m_runtime->registerDynamicVariable(
    VARIABLE_NAME_PREFIX ".z",
    [this](Interpreter* interpreter, const ScriptValue& value) {
        START_ENTITY_BLOCK("Attempted to set z position of null entity")

        auto floatValue = value.getFloatValue();
        entity->setZ(floatValue);

        END_ENTITY_BLOCK
    },
    [this](Interpreter* interpreter, ScriptValue& value) {
        START_SINGLE_ENTITY_BLOCK("Attempted to get z position of null entity")

        value.setFloatValue(entity->getZ());

        END_SINGLE_ENTITY_BLOCK(NOMAD_DEFAULT_FLOAT)
    },
    m_runtime->getFloatType(),
    NomadDoc("The z position (draw order) of the entity.")
);

