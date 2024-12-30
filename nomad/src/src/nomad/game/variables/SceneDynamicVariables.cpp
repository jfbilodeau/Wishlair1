//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define VERIFY_SCENE_NOT_NULL(entity, message) \
    if (entity == nullptr) { \
        log::error(message);  \
        return; \
    }

void Game::init_scene_dynamic_variables() {
    log::debug("Initializing scene dynamic variables");

    m_runtime->register_dynamic_variable("scene.name",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't set scene name, scene is null");

            auto scene_name = value.get_string_value();
            scene->set_name(scene_name);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't get scene name, scene is null");

            auto scene_name = scene->get_name();

            value.set_string_value(scene_name);
        },
        m_runtime->get_string_type(),
        NomadDoc("The name of the current scene")
    );

m_runtime->register_dynamic_variable("scene.z",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't set scene z, scene is null");

            auto scene_z = value.get_integer_value();
            scene->set_z(scene_z);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't get scene z, scene is null");

            auto scene_z = scene->get_z();

            value.set_integer_value(scene_z);
        },
        m_runtime->get_integer_type(),
        NomadDoc("The z-index of the current scene")
    );

    m_runtime->register_dynamic_variable("scene.wall.mask",
         [this](Interpreter* interpreter, const ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't set scene wall mask, scene is null");

            auto wall_mask = value.get_integer_value();
            scene->set_wall_mask(wall_mask);
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            auto scene = get_current_scene();

            VERIFY_SCENE_NOT_NULL(scene, "Can't get scene wall mask, scene is null");

            auto wall_mask = scene->get_wall_mask();

            value.set_integer_value(wall_mask);
        },
        m_runtime->get_integer_type(),
        NomadDoc("The wall mask of the current scene")
    );
}

} // nomad