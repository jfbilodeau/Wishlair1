//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define CHECK_SCENE_NOT_NULL(message) \
    auto scene = getCurrentContext()->getScene(); \
    if (scene == nullptr) { \
        log::error(message); \
        return; \
    }

void Game::initSceneDynamicVariables() {
    log::debug("Initializing scene dynamic variables");

    m_runtime->registerDynamicVariable(
        "scene.camera.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set camera x, scene is null")

            auto camera_x = value.getFloatValue();
            scene->setCameraX(camera_x);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get camera x, scene is null")

            auto camera_x = scene->getCameraX();

            value.setFloatValue(camera_x);
        },
        m_runtime->getFloatType(),
        NomadDoc("The x position of the camera")
    );

    m_runtime->registerDynamicVariable(
        "scene.camera.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set camera y, scene is null")

            auto camera_y = value.getFloatValue();
            scene->setCameraY(camera_y);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get camera y, scene is null")

            auto camera_y = scene->getCameraY();

            value.setFloatValue(camera_y);
        },
        m_runtime->getFloatType(),
        NomadDoc("The y position of the camera")
    );

    m_runtime->registerDynamicVariable("scene.name",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene name, scene is null")

            auto scene_name = value.getStringValue();
            scene->setName(scene_name);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene name, scene is null")

            auto scene_name = scene->getName();

            value.setStringValue(scene_name);
        },
        m_runtime->getStringType(),
        NomadDoc("The name of the current scene")
    );

m_runtime->registerDynamicVariable("scene.z",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene z, scene is null")

            auto scene_z = value.getIntegerValue();
            scene->setZ(scene_z);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene z, scene is null")

            auto scene_z = scene->getZ();

            value.setIntegerValue(scene_z);
        },
        m_runtime->getIntegerType(),
        NomadDoc("The z-index of the current scene")
    );

    m_runtime->registerDynamicVariable("scene.wall.mask",
         [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene wall mask, scene is null")

            auto wall_mask = value.getIntegerValue();
            scene->setWallMask(wall_mask);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene wall mask, scene is null")

            auto wall_mask = scene->getWallMask();

            value.setIntegerValue(wall_mask);
        },
        m_runtime->getIntegerType(),
        NomadDoc("The wall mask of the current scene")
    );
}

} // nomad