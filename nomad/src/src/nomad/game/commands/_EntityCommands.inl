//
// Created by Jean-François Bilodeau on 2023-06-17.
//

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".body.rectangle",
    [this](Interpreter* interpreter) {
        auto bodyType = static_cast<BodyType>(interpreter->getIntegerParameter(0));
        auto width = interpreter->getFloatParameter(1);
        auto height = interpreter->getFloatParameter(2);

        START_ENTITY_BLOCK("Cannot set rectangle body on a null entity")

        entity->setRectangleBody(bodyType, width, height);

        END_ENTITY_BLOCK
    }, {
        defParameter("bodyType", m_runtime->getIntegerType(), NomadParamDoc("Body type.")),
        defParameter("width", m_runtime->getFloatType(), NomadParamDoc("Width of the rectangle body.")),
        defParameter("height", m_runtime->getFloatType(), NomadParamDoc("Height of the rectangle body.")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the rectangle body of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".body.circle",
    [this](Interpreter* interpreter) {
        auto bodyType = static_cast<BodyType>(interpreter->getIntegerParameter(0));
        auto radius = interpreter->getFloatParameter(1);

        START_ENTITY_BLOCK("Cannot set circle body on a null entity")

        entity->setCircleBody(bodyType, radius);

        END_ENTITY_BLOCK
    }, {
        defParameter("bodyType", m_runtime->getIntegerType(), NomadParamDoc("Body type.")),
        defParameter("radius", m_runtime->getFloatType(), NomadParamDoc("Radius of the circle body.")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the circle body of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".body.none",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot set no body on a null entity")

        entity->setNoBody();

        END_ENTITY_BLOCK
    },
    {},
    m_runtime->getVoidType(),
    NomadDoc("Set no body on the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".moveTo",
    [this](Interpreter* interperter) {
        auto x = interperter->getFloatParameter(0);
        auto y = interperter->getFloatParameter(1);
        auto speed = interperter->getFloatParameter(2);
        auto scriptId = interperter->getIdParameter(3);

        START_ENTITY_BLOCK("Cannot call this.moveTo to events on a null entity")

        entity->moveTo(x, y, speed, scriptId);

        END_ENTITY_BLOCK
    },
    {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X coordinate of the destination.")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y coordinate of the destination.")),
        defParameter("speed", m_runtime->getFloatType(), NomadParamDoc("Speed in pixels per seconds to move to the destination.")),
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Move the entity to a specified location.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".on",
    [this](Interpreter* interpreter) {
        NomadString eventName = interpreter->getParameter(0).getStringValue();
        auto scriptId = interpreter->getParameter(1).getIdValue();

        auto scene = getCurrentContext()->getScene();

        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        if (scriptId == NOMAD_INVALID_ID) {
            log::error("Script or function for event '" + eventName + "' not found");
            return;
        }

        scene->registerEntityEvent(eventName, entity->getId(), scriptId);

        END_ENTITY_BLOCK
    }, {
        defParameter("eventName", m_runtime->getStringType(), NomadParamDoc("Name of the event to listen to.")),
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script or function to an event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".on.frame",
    [this](Interpreter* interpreter) {
        auto scriptId = interpreter->getIdParameter(0);

        START_ENTITY_BLOCK("Cannot set on.frame on a null entity")

        if (scriptId == NOMAD_INVALID_ID) {
            log::error("Script for frame event not found");
            return;
        }
        
        entity->setOnFrame(scriptId);

        END_ENTITY_BLOCK
    }, {
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script to the frame event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".on.collisionStart",
    [this](Interpreter* interpreter) {
        auto scriptId = interpreter->getIdParameter(0);

        START_ENTITY_BLOCK("Cannot set on.collisionStart on a null entity")

        if (scriptId == NOMAD_INVALID_ID) {
            log::error("Script for collision start event not found");
            return;
        }

        entity->setOnCollisionBegin(scriptId);

        END_ENTITY_BLOCK
    }, {
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script to the collision start event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".on.collisionEnd",
    [this](Interpreter* interpreter) {
        auto scriptId = interpreter->getIdParameter(0);

        START_ENTITY_BLOCK("Cannot set on.collisionEnd on a null entity")

        if (scriptId == NOMAD_INVALID_ID) {
            log::error("Script or function for collision end event not found");
            return;
        }

        entity->setOnCollisionEnd(scriptId);

        END_ENTITY_BLOCK
    }, {
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script or function to the collision end event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".setLocation",
    [this](Interpreter* interpreter) {
        auto x = interpreter->getFloatParameter(0);
        auto y = interpreter->getFloatParameter(1);

        START_ENTITY_BLOCK("Cannot set location on a null entity")

        entity->setLocation(x, y);

        END_ENTITY_BLOCK
    }, {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X coordinate")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the location of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".trigger",
    [this](Interpreter* interpreter) {
        auto eventName = interpreter->getStringParameter(0);

        auto scene = getCurrentContext()->getScene();

        START_ENTITY_BLOCK("Cannot trigger event on a null entity")

        scene->triggerEvent(eventName, entity);

        END_ENTITY_BLOCK
    }, {
        defParameter("eventName", m_runtime->getStringType(), NomadParamDoc("Name of the event to trigger.")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Trigger an event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".velocity.set",
    [this](Interpreter* interpreter) {
        auto x = interpreter->getFloatParameter(0);
        auto y = interpreter->getFloatParameter(1);

        START_ENTITY_BLOCK("Cannot set velocity on a null entity")

        entity->setVelocity(x, y);

        END_ENTITY_BLOCK
    }, {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X velocity")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y velocity")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the velocity of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".velocity.clear",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot clear velocity on a null entity")

        entity->setVelocity(0, 0);

        END_ENTITY_BLOCK
    },
    {},
    m_runtime->getVoidType(),
    NomadDoc("Clear the velocity of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".getCollidingMask",
    [this](Interpreter* interpreter) {
        START_SINGLE_ENTITY_BLOCK("Cannot get collision mask of a null entity")

        auto scene = entity->getScene();

        auto result = scene->getMaskAtEntity(entity);

        interpreter->setIntegerResult(result);

        END_SINGLE_ENTITY_BLOCK(0)
    },
    {},
    m_runtime->getIntegerType(),
    NomadDoc("Get the collision mask at the entity's position.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".getCollidingMaskAt",
    [this](Interpreter* interpreter) {
        auto x = interpreter->getFloatParameter(0);
        auto y = interpreter->getFloatParameter(1);

        START_SINGLE_ENTITY_BLOCK("Cannot get collision mask of a null entity")

        auto scene = entity->getScene();

        auto result = scene->getMaskAtEntity(entity, {x, y});

        interpreter->setIntegerResult(result);

        END_SINGLE_ENTITY_BLOCK(0)
    },
    {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X coordinate")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->getIntegerType(),
    NomadDoc("Get the collision mask for the entity body at the specified position.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".setSprite",
    [this](Interpreter* interpreter) {
        auto spriteName = interpreter->getStringParameter(0);

        START_ENTITY_BLOCK("Cannot set sprite on a null entity")

        auto sprite = m_resourceManager->getSprites()->getSpriteByName(spriteName);

        entity->setSprite(sprite);

        END_ENTITY_BLOCK
    }, {
        defParameter("name", m_runtime->getStringType(), NomadParamDoc("Name of the sprite to set.")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the sprite of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".setSpriteAnchor",
    [this](Interpreter* interpreter) {
        auto x = interpreter->getFloatParameter(0);
        auto y = interpreter->getFloatParameter(1);

        START_ENTITY_BLOCK("Cannot set sprite anchor on a null entity")

        entity->setLocation(entity->getX() - x, entity->getY() - y);
        entity->setSpriteAnchor(x, y);

        END_ENTITY_BLOCK
    }, {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X coordinate")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the anchor point of the sprite of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".repositionOnAnchor",
    [this](Interpreter* interpreter) {
        auto anchorX = interpreter->getFloatParameter(0);
        auto anchorY = interpreter->getFloatParameter(1);

        START_ENTITY_BLOCK("Cannot reposition on anchor on a null entity")

        auto newX = entity->getX() + anchorX;
        auto newY = entity->getY() + anchorY;

        entity->setLocation(newX, newY);
        // entity->setTextPosition(anchorX, anchorY);
        entity->setSpriteAnchor(anchorX, anchorY);

        END_ENTITY_BLOCK
    },
    {
        defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X coordinate")),
        defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y coordinate")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Reposition the entity based on the new sprite anchor.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".setAnimation",
    [this](Interpreter* interpreter) {
        auto animationName = interpreter->getStringParameter(0);
        auto animationVariant = interpreter->getStringParameter(1);
        auto animationDirection = interpreter->getStringParameter(2);
        auto duration = interpreter->getIntegerParameter(3);
        auto repeat = interpreter->getBooleanParameter(4);
        auto reverse = interpreter->getBooleanParameter(5);

        START_ENTITY_BLOCK("Cannot set animation on a null entity")

        entity->setAnimationDuration(duration);
        entity->setAnimationRepeat(repeat);
        entity->setAnimationReverse(reverse);

        auto animation = m_resourceManager->getAnimations()->getAnimation(
            animationName,
            animationVariant,
            animationDirection
        );

        entity->setAnimation(animation);

        END_ENTITY_BLOCK
    }, {
        defParameter("name", m_runtime->getStringType(), NomadParamDoc("Name of the animation to set.")),
        defParameter("variant", m_runtime->getStringType(), NomadParamDoc("Variant name of the animation to set.")),
        defParameter("direction", m_runtime->getStringType(), NomadParamDoc("Direction name of the animation to set.")),
        defParameter("duration", m_runtime->getIntegerType(), NomadParamDoc("Number of frames to display an animation frame.")),
        defParameter("repeat", m_runtime->getBooleanType(), NomadParamDoc("Whether to repeat the animation.")),
        defParameter("reverse", m_runtime->getBooleanType(), NomadParamDoc("Whether to reverse the animation.")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the animation of the entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".setText",
    [this](Interpreter* interpreter) {
        auto font = interpreter->getIdParameter(0);
        auto text = interpreter->getStringParameter(1);

        START_ENTITY_BLOCK("Cannot set text on a null entity")

        entity->setFontById(font);
        entity->setText(text);

        END_ENTITY_BLOCK
    }, {
        defParameter("fontId", m_runtime->getIntegerType(), NomadParamDoc("ID of the font to use")),
        defParameter("text", m_runtime->getStringType(), NomadParamDoc("Text to set")),
    },
    m_runtime->getVoidType(),
    NomadDoc("Set the font and text to display on an entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".onPress",
    [this](Interpreter* interpreter) {
        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        const auto actionName = interpreter->getStringParameter(0);
        const auto scriptId = interpreter->getIdParameter(1);
        const auto entityId = entity->getId();

        getCurrentContext()->getScene()->addActionPressed(
            actionName,
            scriptId,
            entityId
        );

        END_ENTITY_BLOCK
    }, {
        defParameter("actionName", m_runtime->getStringType(), NomadParamDoc("Name of the action to listen to.")),
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script"))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script or function to the pressed event for this entity.")
);

m_runtime->registerCommand(
    VARIABLE_NAME_PREFIX ".onRelease",
    [this](Interpreter* interpreter) {
        auto actionName = interpreter->getStringParameter(0);
        auto scriptId = interpreter->getIdParameter(1);

        START_ENTITY_BLOCK("Cannot listen to events on a null entity")

        getCurrentContext()->getScene()->addActionReleased(
            actionName,
            scriptId,
            entity->getId()
        );

        END_ENTITY_BLOCK
    }, {
        defParameter("actionName", m_runtime->getStringType(), NomadParamDoc("Name of the action to listen to.")),
        defParameter("callback", m_runtime->getCallbackType({}, m_runtime->getVoidType()), NomadParamDoc("Callback script or function."))
    },
    m_runtime->getVoidType(),
    NomadDoc("Assign a script or function to the release event for this entity.")
);
