//
// Created by jfbilodeau on 23-06-14.
//

#pragma once

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

    [[nodiscard]] NomadId getId() const;

    [[nodiscard]] Scene* getScene() const;

    void setName(const NomadString& name);
    [[nodiscard]] const NomadString& getName() const;

    void setScriptName(const NomadString& scriptName);
    [[nodiscard]] const NomadString& getScriptName() const;

    void setX(NomadFloat x);
    [[nodiscard]] NomadFloat getX() const;

    void setY(NomadFloat y);
    [[nodiscard]] NomadFloat getY() const;

    void setZ(NomadFloat z);
    [[nodiscard]] NomadFloat getZ() const;

    void setLocation(NomadFloat x, NomadFloat y);
    void setLocation(const PointF& location);
    [[nodiscard]] const PointF& getLocation() const;

    void setSize(NomadFloat width, NomadFloat height);
    void setWidth(NomadFloat width);
    [[nodiscard]]
    NomadFloat getWidth() const;
    void setHeight(NomadFloat height);
    [[nodiscard]]
    NomadFloat getHeight() const;
    [[nodiscard]]
    PointF getSize() const;

    void pause();
    void unpause();
    void setPause(bool pause);
    [[nodiscard]] bool isPaused() const;

    void hide();
    void show();
    void setVisible(bool visible);
    [[nodiscard]] bool isVisible() const;
    [[nodiscard]] bool isHidden() const;

    void stopMoving();
    void move(const PointF& velocity);
    void move(NomadFloat x, NomadFloat y);
    void startMovingInDirection(NomadFloat angle, NomadFloat speed);
    void moveTo(const PointF& destination, NomadFloat speed, NomadId onArriveAtDestination = NOMAD_INVALID_ID);
    void moveTo(NomadFloat x, NomadFloat y, NomadFloat speed, NomadId onArriveAtDestination = NOMAD_INVALID_ID);
    [[nodiscard]]
    bool isMoving() const;

    void setVelocity(NomadFloat x, NomadFloat y);
    void setVelocity(const PointF& velocity);
    void setVelocity(Cardinal direction, NomadFloat speed);
    void setVelocityX(NomadFloat x);
    void setVelocityY(NomadFloat y);
    [[nodiscard]] const PointF& getVelocity() const;

    void setDestination(NomadFloat x, NomadFloat y);
    void setDestination(const PointF& destination);
    [[nodiscard]] const PointF& getDestination() const;

    void setDestinationX(NomadFloat x);
    [[nodiscard]] NomadFloat get_destination_x() const;

    void set_destination_y(NomadFloat y);
    [[nodiscard]] NomadFloat getDestinationY() const;

    void setSpeed(NomadFloat speed);
    [[nodiscard]] NomadFloat getSpeed() const;

    void setMask(NomadInteger mask);
    [[nodiscard]] NomadInteger getMask() const;

    void setCollisionMask(NomadInteger collisionMask);
    [[nodiscard]] NomadInteger getCollisionMask() const;

    void setSensor(bool isSensor);
    [[nodiscard]] bool isSensor() const;

    void setNoBody();
    void setCircleBody(BodyType body_type, NomadFloat radius);
    void setRectangleBody(BodyType body_type, NomadFloat width, NomadFloat height);

    [[nodiscard]] BodyType getBodyType() const;
    [[nodiscard]] BodyShape getBodyShape() const;
    [[nodiscard]] NomadFloat getBodyWidth() const;
    [[nodiscard]] NomadFloat getBodyHeight() const;
    [[nodiscard]] NomadFloat getBodyRadius() const;

    [[nodiscard]] bool isTouching(const RectangleF& rectangle) const;
    [[nodiscard]] bool isTouching(const CircleF& circle) const;
    [[nodiscard]] bool isTouching(const Entity* entity) const;

    RectangleF& getBoundingBox(RectangleF& rectangle) const;

    // Notify the entity that it has entered the camera frame.
    void enterCamera();
    // Notify the entity that it has exited the camera frame.
    void exitCamera();

    // Script to execute when the entity enters the camera frame.
    void setOnEnterCamera(NomadId scriptId);
    [[nodiscard]] NomadId getOnEnterCamera() const;

    // Script to execute when the entity exits the camera frame.
    void setOnExitCamera(NomadId scriptId);
    [[nodiscard]] NomadId getOnExitCamera() const;

    [[nodiscard]] bool isInCamera() const;

    void invalidatePhysicsBody();
    void beforeSimulationUpdate(b2WorldId world);
    void afterSimulationUpdate(b2WorldId world);

    void setLayer(NomadInteger layer);
    [[nodiscard]] NomadInteger getLayer() const;

    void setSpriteName(const NomadString& spriteName);
    [[nodiscard]] const NomadString& getSpriteName() const;

    void setSprite(const Sprite* sprite);
    [[nodiscard]] const Sprite* getSprite() const;

    void setSpriteX(NomadFloat x);
    [[nodiscard]] NomadFloat getSpriteX() const;

    void setSpriteY(NomadFloat y);
    [[nodiscard]] NomadFloat getSpriteY() const;

    void setSpriteAnchor(const PointF& anchor);
    void setSpriteAnchor(NomadFloat x, NomadFloat y);
    [[nodiscard]] const PointF& getSpriteAnchor() const;

    void setAnimation(const Animation* animation);
    [[nodiscard]] const Animation* getAnimation() const;

    void setAnimationName(const NomadString& animationName);
    [[nodiscard]] const NomadString& getAnimationName() const;

    void setAnimationVariant(const NomadString& animationVariant);
    [[nodiscard]] const NomadString& getAnimationVariant() const;

    void setAnimationDirection(const NomadString& animationDirection);
    [[nodiscard]] const NomadString& getAnimationDirection() const;

    void setAnimationDuration(NomadInteger speed);
    [[nodiscard]] NomadInteger getAnimationDuration() const;

    void setAnimationRepeat(bool repeat);
    [[nodiscard]] bool getAnimationRepeat() const;

    void setAnimationReverse(bool reverse);
    [[nodiscard]] bool getAnimationReverse() const;

    void setText(const NomadString& text);
    [[nodiscard]] const NomadString& getText();

    void setTextAlignment(Alignment alignment);
    [[nodiscard]] Alignment getTextAlignment() const;

    void setTextPosition(NomadFloat x, NomadFloat y);
    void setTextPosition(const PointF& position);
    [[nodiscard]] const PointF& getTextPosition() const;

    void setTextX(NomadFloat x);
    [[nodiscard]] NomadFloat getTextX() const;

    void setTextY(NomadFloat y);
    [[nodiscard]] NomadFloat getTextY() const;

    void setTextWidth(NomadFloat width);
    [[nodiscard]] NomadFloat getTextWidth() const;

    void setTextHeight(NomadFloat height);
    [[nodiscard]] NomadFloat getTextHeight() const;

    void setTextLineSpacing(NomadFloat lineSpacing);
    [[nodiscard]] NomadFloat getTextLineSpacing() const;

    void setTextColor(const Color& color);
    [[nodiscard]] Color getTextColor() const;

    void setFontById(NomadId fontId);
    [[nodiscard]] NomadId getFontId() const;

    void setVariableValue(NomadId variableId, const ScriptValue& value);
    void getVariableValue(NomadId variableId, ScriptValue& value) const;

    void addEvent(const NomadString& name, NomadId scriptId);
    void removeEvent(const NomadString& name);

    GameExecutionContext* getExecutionContext();

    [[nodiscard]] NomadId getOnFrame() const;
    void setOnFrame(NomadId scriptId);

    [[nodiscard]] NomadId getOnCollisionBegin() const;
    void setOnCollisionBegin(NomadId scriptId);

    [[nodiscard]] NomadId getOnCollisionEnd() const;
    void setOnCollisionEnd(NomadId scriptId);

private:
    NomadId m_id = NOMAD_INVALID_ID;
    Scene* m_scene;
    NomadString m_scriptName;
    NomadString m_name;

    // World position
    PointF m_position;
    PointF m_size;
    NomadFloat m_z = 0.0;
    NomadInteger m_layer = 0;

    // State
    NomadBoolean m_paused = false;
    NomadBoolean m_visible = true;

    // Movement
    PointF m_velocity = {};
    bool m_moveToDestination = false;
    PointF m_destination = {};
    NomadFloat m_speed = 0;
    NomadId m_onArriveAtDestination = NOMAD_INVALID_ID;

    // Mask, body and collision
    BodyShape m_bodyShape = BodyShape::None;
    BodyType m_bodyType = BodyType::Static;
    NomadBoolean m_isSensor = false;
    NomadFloat m_bodyWidth = 0;
    NomadFloat m_bodyHeight = 0;
    NomadFloat m_bodyRadius = 0;
    b2BodyId m_b2Body = {};
    b2ShapeId m_b2Shape = {};
    b2Filter m_b2Filter = b2DefaultFilter();
    NomadBoolean m_hasBody = false;
    NomadBoolean m_bodyInvalidated = true;
    NomadBoolean m_positionInvalidated = true;
    NomadBoolean m_velocityInvalidated = true;

    // Camera
    bool m_inCamera = false;

    // Visuals
    PointF m_spriteAnchor;  // Sprite image anchor point
    const Sprite* m_sprite = nullptr;
    const Animation* m_animation = nullptr;
    NomadInteger m_currentFrame = 0;
    NomadInteger m_frameCount = 0;

    // Animation
    NomadString m_animationName;
    NomadString m_animationVariant = "idle";
    NomadString m_animationDirection = "south";
    NomadInteger m_animationDuration = 1;
    NomadBoolean m_animationRepeat = true;
    NomadBoolean m_animationReverse = false;
    bool m_animationDirty = false;

    // Text
    NomadString m_text;
    NomadString m_wrappedText;
    Alignment m_textAlignment = Alignment::CenterMiddle;
    PointF m_textPosition;
    NomadFloat m_textWidth = 0;
    NomadFloat m_textHeight = 0;
    NomadFloat m_textLineSpacing = 0;
    NomadId m_fontId = NOMAD_INVALID_ID;
    Color m_textColor = Colors::Black;
    Texture* m_textTexture = nullptr;

    // Variable / Script
    VariableList m_variables;
    EventManager m_events;
    GameExecutionContext m_executionContext;

    // Events
    NomadId m_onFrame = NOMAD_INVALID_ID;
    NomadId m_onCollisionBegin = NOMAD_INVALID_ID;
    NomadId m_onCollisionEnd = NOMAD_INVALID_ID;
    NomadId m_onEnterCamera = NOMAD_INVALID_ID;
    NomadId m_onExitCamera = NOMAD_INVALID_ID;

    void invalidateTextTexture();
    void generateTextTexture(Canvas* canvas);
    void destroyBody();
};

using EntityList = std::vector<Entity*>;

} // nomad

