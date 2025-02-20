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
    auto scene = get_current_context()->get_scene(); \
    if (scene == nullptr) { \
        log::error(message); \
        return; \
    }

void Game::init_scene_dynamic_variables() {
    log::debug("Initializing scene dynamic variables");

    m_runtime->register_dynamic_variable(
        "scene.camera.x",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set camera x, scene is null")

            auto camera_x = value.get_float_value();
            scene->set_camera_x(camera_x);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get camera x, scene is null")

            auto camera_x = scene->get_camera_x();

            value.set_float_value(camera_x);
        },
        m_runtime->get_float_type(),
        NomadDoc("The x position of the camera")
    );

    m_runtime->register_dynamic_variable(
        "scene.camera.y",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set camera y, scene is null")

            auto camera_y = value.get_float_value();
            scene->set_camera_y(camera_y);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get camera y, scene is null")

            auto camera_y = scene->get_camera_y();

            value.set_float_value(camera_y);
        },
        m_runtime->get_float_type(),
        NomadDoc("The y position of the camera")
    );

    m_runtime->register_dynamic_variable("scene.name",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene name, scene is null")

            auto scene_name = value.get_string_value();
            scene->set_name(scene_name);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene name, scene is null")

            auto scene_name = scene->get_name();

            value.set_string_value(scene_name);
        },
        m_runtime->get_string_type(),
        NomadDoc("The name of the current scene")
    );

m_runtime->register_dynamic_variable("scene.z",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene z, scene is null")

            auto scene_z = value.get_integer_value();
            scene->set_z(scene_z);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene z, scene is null")

            auto scene_z = scene->get_z();

            value.set_integer_value(scene_z);
        },
        m_runtime->get_integer_type(),
        NomadDoc("The z-index of the current scene")
    );

    m_runtime->register_dynamic_variable("scene.wall.mask",
         [this](Interpreter* interpreter, const ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't set scene wall mask, scene is null")

            auto wall_mask = value.get_integer_value();
            scene->set_wall_mask(wall_mask);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            CHECK_SCENE_NOT_NULL("Can't get scene wall mask, scene is null")

            auto wall_mask = scene->get_wall_mask();

            value.set_integer_value(wall_mask);
        },
        m_runtime->get_integer_type(),
        NomadDoc("The wall mask of the current scene")
    );
}

} // nomad