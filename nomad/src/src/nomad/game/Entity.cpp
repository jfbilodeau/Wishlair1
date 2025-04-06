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
    m_executionContext.reset(scene, this);
}

Entity::~Entity() {
    invalidateTextTexture(); // Clear the text texture if it exists
}

Scene* Entity::getScene() const {
    return m_scene;
}

void Entity::setName(const NomadString& name) {
    m_name = name;
}

const NomadString& Entity::getName() const {
    return m_name;
}

void Entity::setScriptName(const NomadString& scriptName) {
    m_scriptName = scriptName;
}

const NomadString& Entity::getScriptName() const {
    return m_scriptName;
}

void Entity::setX(const NomadFloat x) {
    m_position.setX(x);
    m_destination.setX(x);
    m_positionInvalidated = true;
}

NomadFloat Entity::getX() const {
    return m_position.getX();
}

void Entity::setY(NomadFloat y) {
    m_position.setY(y);
    m_destination.setY(y);
    m_positionInvalidated = true;
}

NomadFloat Entity::getY() const {
    return m_position.getY();
}

void Entity::setZ(NomadFloat z) {
    m_z = z;
}

NomadFloat Entity::getZ() const {
    return m_z;
}

void Entity::setLocation(NomadFloat x, NomadFloat y) {
    m_position.set(x, y);
    m_positionInvalidated = true;
}

void Entity::setLocation(const PointF& location) {
    m_position.set(location);
}

const PointF& Entity::getLocation() const {
    return m_position;
}

void Entity::setSize(NomadFloat width, NomadFloat height) {
    m_size.set(width, height);
}

void Entity::setWidth(NomadFloat width) {
    m_size.setY(width);
}

NomadFloat Entity::getWidth() const {
    return m_size.getY();
}

void Entity::setHeight(NomadFloat height) {
    m_size.setX(height);
}

NomadFloat Entity::getHeight() const {
    return m_size.getX();
}

PointF Entity::getSize() const {
    return m_size;
}

void Entity::pause() {
    m_paused = true;
}

void Entity::unpause() {
    m_paused = false;
}

void Entity::setPause(bool pause) {
    m_paused = pause;
}

bool Entity::isPaused() const {
    return m_paused;
}

void Entity::hide() {
    m_visible = false;
}

void Entity::show() {
    m_visible = true;
}

void Entity::setVisible(bool visible) {
    m_visible = visible;
}

bool Entity::isVisible() const {
    return m_visible;
}

bool Entity::isHidden() const {
    return !m_visible;
}

void Entity::stopMoving() {
    m_velocity.zero();
    m_velocityInvalidated = true;
    m_moveToDestination = false;
}

void Entity::move(const PointF& velocity) {
    m_velocity.set(velocity);
    m_velocityInvalidated = true;
    m_moveToDestination = false;
}

void Entity::move(NomadFloat x, NomadFloat y) {
    m_velocity.set(x, y);
    m_velocityInvalidated = true;
    m_moveToDestination = false;
}

void Entity::startMovingInDirection(NomadFloat angle, NomadFloat speed) {
    m_velocity.set(
        std::cos(angle) * speed,
        std::sin(angle) * speed
    );
    m_velocityInvalidated = true;
    m_moveToDestination = false;
}

void Entity::moveTo(const PointF& destination, NomadFloat speed, NomadId onArriveAtDestination) {
    moveTo(destination.getX(), destination.getY(), speed, onArriveAtDestination);
}

void Entity::moveTo(NomadFloat x, NomadFloat y, NomadFloat speed, NomadId onArriveAtDestination) {
    m_destination.set(x, y);
    m_speed = speed;
    m_velocityInvalidated = true;
    m_moveToDestination = true;
    m_onArriveAtDestination = onArriveAtDestination;
}

bool Entity::isMoving() const {
    return m_velocity.is_zero();
}

void Entity::setVelocity(NomadFloat x, NomadFloat y) {
    setVelocity({x, y});
}

void Entity::setVelocity(const PointF& velocity) {
    m_velocity.set(velocity);
    m_velocityInvalidated = true;
}

void Entity::setVelocity(Cardinal direction, NomadFloat speed) {
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
    m_velocityInvalidated = true;
}

void Entity::setVelocityX(NomadFloat x) {
    m_velocity.setX(x);
    m_velocityInvalidated = true;
}

void Entity::setVelocityY(NomadFloat y) {
    m_velocity.setY(y);
    m_velocityInvalidated = true;
}

const PointF& Entity::getVelocity() const {
    return m_velocity;
}

void Entity::setDestination(NomadFloat x, NomadFloat y) {
    m_destination.set(x, y);
}

void Entity::setDestination(const PointF& destination) {
    m_destination.set(destination);
}

const PointF& Entity::getDestination() const {
    return m_destination;
}

void Entity::setDestinationX(NomadFloat x) {
    m_destination.setX(x);
}

NomadFloat Entity::get_destination_x() const {
    return m_destination.getX();
}

void Entity::set_destination_y(NomadFloat y) {
    m_destination.setY(y);
}

NomadFloat Entity::getDestinationY() const {
    return m_destination.getY();
}

void Entity::setSpeed(NomadFloat speed) {
    m_speed = speed;
}

NomadFloat Entity::getSpeed() const {
    return m_speed;
}

void Entity::setMask(const NomadInteger mask) {
//    m_mask = mask;
    m_b2Filter.categoryBits = mask;
}

NomadInteger Entity::getMask() const {
    return m_b2Filter.categoryBits;
}

void Entity::setCollisionMask(const NomadInteger collisionMask) {
    m_b2Filter.maskBits = collisionMask;
}

[[nodiscard]] NomadInteger Entity::getCollisionMask() const {
    return m_b2Filter.maskBits;
}

void Entity::setSensor(bool isSensor) {
    m_isSensor = isSensor;

    invalidatePhysicsBody();
}

bool Entity::isSensor() const {
    return m_isSensor;
}

void Entity::setNoBody() {
    invalidatePhysicsBody();

    m_bodyShape = BodyShape::None;
}

void Entity::setCircleBody(BodyType body_type, NomadFloat radius) {
    invalidatePhysicsBody();

    m_bodyShape = BodyShape::Circle;
    m_bodyType = body_type;
    m_bodyRadius = radius;
}

void Entity::setRectangleBody(BodyType body_type, NomadFloat width, NomadFloat height) {
    invalidatePhysicsBody();

    m_bodyShape = BodyShape::Rectangle;
    m_bodyType = body_type;
    m_bodyWidth = width;
    m_bodyHeight = height;
}

BodyType Entity::getBodyType() const {
    return m_bodyType;
}

BodyShape Entity::getBodyShape() const {
    return m_bodyShape;
}

NomadFloat Entity::getBodyWidth() const {
    return m_bodyWidth;
}

NomadFloat Entity::getBodyHeight() const {
    return m_bodyHeight;
}

NomadFloat Entity::getBodyRadius() const {
    return m_bodyRadius;
}

bool Entity::isTouching(const RectangleF& rectangle) const {
    if (m_bodyShape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            m_position.getX() - m_bodyWidth / 2,
            m_position.getY() - m_bodyHeight / 2,
            m_bodyWidth,
            m_bodyHeight
        );

        return rectangleRectangleIntersect(entity_rectangle, rectangle);
    } else if (m_bodyShape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            m_position.getX(),
            m_position.getY(),
            m_bodyRadius
        );

        return circleRectangleIntersect(entity_circle, rectangle);
    }

    return false;
}

bool Entity::isTouching(const CircleF& circle) const {
    if (m_bodyShape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            m_position.getX() - m_bodyWidth / 2,
            m_position.getY() - m_bodyHeight / 2,
            m_bodyWidth,
            m_bodyHeight
        );

        return circleRectangleIntersect(circle, entity_rectangle);
    } else if (m_bodyShape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            m_position.getX(),
            m_position.getY(),
            m_bodyRadius
        );

        return circleCircleIntersect(entity_circle, circle);
    }

    return false;
}

bool Entity::isTouching(const Entity* entity) const {
    auto entity_body_shape = entity->getBodyShape();

    if (entity_body_shape == BodyShape::Rectangle) {
        auto entity_rectangle = RectangleF(
            entity->getX() - entity->getBodyWidth() / 2,
            entity->getY() - entity->getBodyHeight() / 2,
            entity->getBodyWidth(),
            entity->getBodyHeight()
        );

        return isTouching(entity_rectangle);
    } else if (entity_body_shape == BodyShape::Circle) {
        auto entity_circle = CircleF(
            entity->getX(),
            entity->getY(),
            entity->getBodyRadius()
        );

        return isTouching(entity_circle);
    }

    return false;
}

RectangleF& Entity::getBoundingBox(RectangleF &bounding_box) const {
    bounding_box = RectangleF{
        m_position.getX(),
        m_position.getY(),
        0.0f,
        0.0f
    };

    if (auto sprite = getSprite()) {
        bounding_box = sprite->getFrame().toRectanglef();
    }

    switch (m_bodyShape) {
        case BodyShape::None:
            // Ignore
            break;
        case BodyShape::Rectangle:
            bounding_box = bounding_box.unionRect({
                m_position.getX() - m_bodyWidth / 2,
                m_position.getY() - m_bodyHeight / 2,
                m_bodyWidth,
                m_bodyHeight
            });
            break;
        case BodyShape::Circle:
            bounding_box = bounding_box.unionRect({
                m_position.getX() - m_bodyWidth / 2,
                m_position.getY() - m_bodyHeight / 2,
                m_bodyRadius,
                m_bodyRadius
            });
            break;
        default:
            log::error("[Entity::get_bounding_box] Invalid body shape: " + toString(static_cast<int>(m_bodyShape)));
    }

    return bounding_box;
}

void Entity::enterCamera() {
    if (m_inCamera) {
        return;
    }

    if (m_onEnterCamera != NOMAD_INVALID_ID) {
        m_scene->getGame()->executeScriptInContext(m_onEnterCamera, &m_executionContext);
    }
}

void Entity::exitCamera() {
    if (!m_inCamera) {
        return;
    }

    if (m_onExitCamera != NOMAD_INVALID_ID) {
        m_scene->getGame()->executeScriptInContext(m_onExitCamera, &m_executionContext);
    }
}

void Entity::setOnEnterCamera(NomadId scriptId) {
    m_onEnterCamera = scriptId;
}

NomadId Entity::getOnEnterCamera() const {
    return m_onEnterCamera;
}

void Entity::setOnExitCamera(NomadId scriptId) {
    m_onExitCamera = scriptId;
}

NomadId Entity::getOnExitCamera() const {
    return m_onExitCamera;
}

bool Entity::isInCamera() const {
    return m_inCamera;
}

void Entity::invalidatePhysicsBody() {
    m_bodyInvalidated = true;
}

void Entity::beforeSimulationUpdate(b2WorldId world) {
    if (m_bodyInvalidated) {
        if (m_hasBody) {
            b2DestroyBody(m_b2Body);
            m_hasBody = false;
        }

        m_bodyInvalidated = false;
    
        if (m_bodyShape != BodyShape::None) {
            b2BodyDef body_def = b2DefaultBodyDef();
            body_def.userData = this;

            if (m_isSensor) {
                if (m_bodyType != BodyType::Static) {
                    log::warning("Sensors must be static bodies");
                }

                body_def.type = b2_staticBody;
            } else if (m_bodyType == BodyType::Static) {
                body_def.type = b2_staticBody;
            } else if (m_bodyType == BodyType::Dynamic) {
                body_def.type = b2_dynamicBody;
            } else if (m_bodyType == BodyType::Kinematic) {
                body_def.type = b2_kinematicBody;
            } else {
                log::error("Invalid body type: " + toString(static_cast<int>(m_bodyType)));
                return;
            }

            // Create collision body.
            m_b2Body = b2CreateBody(world, &body_def);

            if (b2Body_IsValid(m_b2Body)) {
                m_hasBody = true;
            } else {
                log::error("Failed to create body");
                return;
            }

            b2ShapeDef shape_def = b2DefaultShapeDef();

            shape_def.userData = this;
            shape_def.isSensor = m_isSensor;
            shape_def.filter = m_b2Filter;

            if (m_bodyShape == BodyShape::Rectangle) {
                b2Polygon rectangle = b2MakeBox(
                    static_cast<float>(m_bodyWidth) / 2.0f,
                    static_cast<float>(m_bodyHeight) / 2.0f
                );
                m_b2Shape = b2CreatePolygonShape(m_b2Body, &shape_def, &rectangle);
            } else if (m_bodyShape == BodyShape::Circle) {
                b2Circle circle = {
                    {0.0f, 0.0f},
                    static_cast<float>(m_bodyRadius)
                };
                m_b2Shape = b2CreateCircleShape(m_b2Body, &shape_def, &circle);
            } else {
                log::error("Invalid body shape: " + toString(static_cast<int>(m_bodyShape)));
                return;
            }

            if (b2Shape_IsValid(m_b2Shape) == false) {
                log::error("Failed to create shape");
                return;
            }
        }
    }

    if (m_hasBody) {
        if (m_positionInvalidated) {
            // Make sure body is at the same position as the entity
            b2Body_SetTransform(m_b2Body, b2Vec2{
                static_cast<float>(m_position.getX()),
                static_cast<float>(m_position.getY())
            }, b2Rot_identity);
        }

        if  (m_velocityInvalidated) {
            b2Body_SetLinearVelocity(m_b2Body, b2Vec2{
                static_cast<float>(m_velocity.getX()),
                static_cast<float>(m_velocity.getY())
            });
        }
    }

    m_positionInvalidated = false;
    m_velocityInvalidated = false;
}

void Entity::afterSimulationUpdate(b2WorldId world) {
    // Update entity position based on physics body.
    if (m_hasBody) {
        auto position = b2Body_GetPosition(m_b2Body);
        m_position.set(position.x, position.y);
    } else {
        // Manually update velocity.
        m_position.translate(m_velocity);
    }
}

void Entity::setLayer(const NomadInteger layer) {
    m_layer = layer;
    invalidatePhysicsBody();
}

NomadInteger Entity::getLayer() const {
    return m_layer;
}

void Entity::setSpriteName(const NomadString& spriteName) {
    auto sprite = m_scene->getGame()->getResources()->getSprites()->getSpriteByName(spriteName);

    if (sprite == nullptr) {
        log::warning("Sprite '" + spriteName + "' not found");
        return;
    }

    setSprite(sprite);
}

const NomadString& Entity::getSpriteName() const {
    if (m_sprite == nullptr) {
        return NOMAD_EMPTY_STRING;
    }

    return m_sprite->getName();
}

void Entity::update(Scene* scene) {
    auto game = scene->getGame();

    if (m_onFrame != NOMAD_INVALID_ID) {
        game->executeScriptInContext(m_onFrame, &m_executionContext);
    }

    // Update movement.
    if (m_moveToDestination) {
        auto distance = m_position.distanceTo(m_destination);
        auto speed = m_speed / m_scene->getGame()->getFps();

        if (distance > speed) {
            auto angle = m_position.angleTo(m_destination);

            auto velocity_x = std::cos(angle) * speed;
            auto velocity_y = std::sin(angle) * speed;

            setVelocity(velocity_x, velocity_y);
        } else {
            setLocation(m_destination);
            setVelocity(0, 0);
            m_moveToDestination = false;

            if (m_onArriveAtDestination != NOMAD_INVALID_ID) {
                game->executeScriptInContext(m_onArriveAtDestination, &m_executionContext);
            }
        }
    }

    // Do we need to select new animation?
    if (m_animationDirty) {
        m_animationDirty = false;

        auto animation = game->getResources()->getAnimations()->getAnimation(
            m_animationName,
            m_animationVariant,
            m_animationDirection
        );

        if (animation == nullptr) {
            log::warning(
                "Animation '" + m_animationName + "_" + m_animationVariant + "_" + m_animationDirection +
                "' not found"
            );
        }

        setAnimation(animation);
    }

    // Update animation
    if (m_animation) {
        m_frameCount--;

        if (m_frameCount <= 0) {
            if (m_animationReverse) {
                m_currentFrame--;

                if (m_currentFrame <= 0) {
                    if (m_animationRepeat) {
                        m_currentFrame = m_animation->getFrameCount() - 1;
                    } else {
                        m_currentFrame = 0;
                    }
                }
            } else {
                m_currentFrame++;

                if (m_currentFrame >= m_animation->getFrameCount()) {
                    if (m_animationRepeat) {
                        m_currentFrame = 0;
                    } else {
                        m_currentFrame = m_animation->getFrameCount() - 1;
                    }
                }
            }

            m_frameCount = m_animationDuration;

            const auto sprite = m_animation->getFrame(m_currentFrame);

            setSprite(sprite);
        }
    }
}

void Entity::render(Canvas* canvas) {
    if (m_visible == false) {
        return;
    }

    NomadFloat entity_x = getX();
    NomadFloat entity_y = getY();

    if (m_sprite != nullptr) {
        auto sprite_x = entity_x - getSpriteX();
        auto sprite_y = entity_y - getSpriteY();

        canvas->renderSprite(m_sprite, sprite_x, sprite_y);
    }

    if (m_textTexture == nullptr && !m_text.empty()) {
        generateTextTexture(canvas);
    }

    if (m_textTexture != nullptr) {
        Point anchor;

        const auto text_x = static_cast<int>(m_textPosition.getX());
        const auto text_y = static_cast<int>(m_textPosition.getY());

        const int text_width = m_textTexture->getWidth();
        const int text_height = m_textTexture->getHeight();

        switch (m_textAlignment) {
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
                    toString(static_cast<int>(m_textAlignment)) +
                    " for entity '" +
                    m_name +
                    "'"
                );
        }

        auto source = Rectangle{
            static_cast<NomadInteger>(0.0),
            static_cast<NomadInteger>(0.0),
            text_width,
            text_height
        };

        auto destination = Rectangle{
            anchor.getX() + static_cast<int>(entity_x + m_textPosition.getX()),
            anchor.getY() + static_cast<int>(entity_y + m_textPosition.getY()),
            text_width,
            text_height
        };

        canvas->renderTexture(m_textTexture, source, destination);
    }
}

NomadId Entity::getId() const {
    return m_id;
}

void Entity::setSprite(const Sprite* sprite) {
    if (sprite == m_sprite) {
        return;
    }

    m_sprite = sprite;
}

const Sprite* Entity::getSprite() const {
    return m_sprite;
}

void Entity::setSpriteX(NomadFloat x) {
    m_spriteAnchor.setX(x);
}

NomadFloat Entity::getSpriteX() const {
    return m_spriteAnchor.getX();
}

void Entity::setSpriteY(NomadFloat y) {
    m_spriteAnchor.setY(y);
}

NomadFloat Entity::getSpriteY() const {
    return m_spriteAnchor.getY();
}

void Entity::setSpriteAnchor(const PointF& anchor) {
    m_spriteAnchor = anchor;
}

void Entity::setSpriteAnchor(NomadFloat x, NomadFloat y) {
    m_spriteAnchor.set(x, y);
}

const PointF& Entity::getSpriteAnchor() const {
    return m_spriteAnchor;
}

void Entity::setAnimation(const Animation* animation) {
    if (animation == m_animation) {
        return;
    }

    m_animation = animation;
    m_frameCount = 0;

    if (m_animation == nullptr) {
        setSprite(nullptr);
        return;
    }

    if (m_animationReverse) {
        setSprite(m_animation->getLastFrame());
    } else {
        setSprite(m_animation->getFrame(0));
    }
}

[[nodiscard]] const Animation* Entity::getAnimation() const {
    return m_animation;
}

void Entity::setAnimationName(const NomadString& animationName) {
    if (animationName == m_animationName) {
        return;
    }

    m_animationName = animationName;
    m_animationDirty = true;
}

const NomadString& Entity::getAnimationName() const {
    return m_animationName;
}

void Entity::setAnimationVariant(const NomadString& animationVariant) {
    if (animationVariant == m_animationVariant) {
        return;
    }

    m_animationVariant = animationVariant;
    m_animationDirty = true;
}

const NomadString& Entity::getAnimationVariant() const {
    return m_animationVariant;
}

void Entity::setAnimationDirection(const NomadString& animationDirection) {
    if (animationDirection == m_animationDirection) {
        return;
    }

    m_animationDirection = animationDirection;
    m_animationDirty = true;
}

const NomadString& Entity::getAnimationDirection() const {
    return m_animationDirection;
}

void Entity::setAnimationDuration(NomadInteger speed) {
    if (speed == m_animationDuration) {
        return;
    }

    m_animationDuration = speed;
}

NomadInteger Entity::getAnimationDuration() const {
    return m_animationDuration;
}

void Entity::setAnimationRepeat(bool repeat) {
    if (repeat == m_animationRepeat) {
        return;
    }

    m_animationRepeat = repeat;
}

bool Entity::getAnimationRepeat() const {
    return m_animationRepeat;
}

void Entity::setAnimationReverse(bool reverse) {
    if (reverse == m_animationReverse) {
        return;
    }

    m_animationReverse = reverse;
}

bool Entity::getAnimationReverse() const {
    return m_animationReverse;
}

void Entity::setText(const NomadString& text) {
    if (text != m_text) {
        m_text = text;

        invalidateTextTexture();
    }
}

const NomadString& Entity::getText() {
    return m_text;
}

void Entity::setTextAlignment(Alignment alignment) {
    m_textAlignment = alignment;
}

Alignment Entity::getTextAlignment() const {
    return m_textAlignment;
}

void Entity::setTextPosition(NomadFloat x, NomadFloat y) {
    m_textPosition = { x, y };
}

void Entity::setTextPosition(const PointF &position) {
    m_textPosition = position;
}

const PointF & Entity::getTextPosition() const {
    return m_textPosition;
}

void Entity::setTextX(NomadFloat x) {
    m_textPosition.setX(x);
}

NomadFloat Entity::getTextX() const {
    return m_textPosition.getX();
}

void Entity::setTextY(NomadFloat y) {
    m_textPosition.setY(y);
}

NomadFloat Entity::getTextY() const {
    return m_textPosition.getY();
}

void Entity::setTextWidth(NomadFloat width) {
    if (width == m_textWidth) {
        return;
    }

    m_textWidth = width;

    invalidateTextTexture();
}

NomadFloat Entity::getTextWidth() const {
    return m_textWidth;
}

void Entity::setTextHeight(NomadFloat height) {
    if (height == m_textHeight) {
        return;
    }

    m_textHeight = height;

    invalidateTextTexture();
}

NomadFloat Entity::getTextHeight() const {
    return m_textHeight;
}

void Entity::setTextLineSpacing(NomadFloat lineSpacing) {
    if (lineSpacing == m_textLineSpacing) {
        return;
    }

    m_textLineSpacing = lineSpacing;

    invalidateTextTexture();
}

NomadFloat Entity::getTextLineSpacing() const {
    return m_textLineSpacing;
}

void Entity::setTextColor(const Color& color) {
    if (color == m_textColor) {
        return;
    }
    m_textColor = color;

    invalidateTextTexture();
}

Color Entity::getTextColor() const {
    return m_textColor;
}

void Entity::setFontById(NomadId fontId) {
    if (fontId == m_fontId) {
        return;
    }

    m_fontId = fontId;

    invalidateTextTexture();
}

NomadId Entity::getFontId() const {
    return m_fontId;
}

void Entity::setVariableValue(NomadId variableId, const ScriptValue& value) {
    m_variables.setVariableValue(variableId, value);
}

void Entity::getVariableValue(NomadId variableId, ScriptValue& value) const {
    m_variables.getVariableValue(variableId, value);
}

void Entity::addEvent(const NomadString& name, NomadId scriptId) {
    m_events.addEvent(name, scriptId);
}

void Entity::removeEvent(const NomadString& name) {
    m_events.removeEvent(name);
}

GameExecutionContext* Entity::getExecutionContext() {
    return &m_executionContext;
}

NomadId Entity::getOnFrame() const {
    return m_onFrame;
}

void Entity::setOnFrame(NomadId scriptId) {
    m_onFrame = scriptId;
}

NomadId Entity::getOnCollisionBegin() const {
    return m_onCollisionBegin;
}

void Entity::setOnCollisionBegin(NomadId scriptId) {
    m_onCollisionBegin = scriptId;
}

NomadId Entity::getOnCollisionEnd() const {
    return m_onCollisionEnd;
}

void Entity::setOnCollisionEnd(NomadId scriptId) {
    m_onCollisionEnd = scriptId;
}

void Entity::invalidateTextTexture() {
    if (m_textTexture) {
        delete m_textTexture;

        m_textTexture = nullptr;
    }
}

void Entity::generateTextTexture(Canvas* canvas) {
    if (m_textTexture) {
        delete m_textTexture;

        m_textTexture = nullptr;
    }

    if (m_fontId == NOMAD_INVALID_ID) {
        log::warning("No font set for entity '" + m_name + "'");
    } else {
        auto font = canvas->getGame()->getResources()->getFonts()->getFont(m_fontId);

        m_textTexture = font->generateTexture(
            canvas,
            m_text,
            m_textColor,
            getHorizontalAlignment(m_textAlignment),
            static_cast<NomadInteger>(m_textWidth),
            static_cast<NomadInteger>(m_textHeight),
            static_cast<NomadInteger>(m_textLineSpacing)
        );
    }
}

} // namespace nomad