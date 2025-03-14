//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/Animation.hpp"
#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Sprite.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define VARIABLE_NAME_PREFIX "other"

#define START_ENTITY_BLOCK(message) \
        get_current_context()->for_each_other_entities([&,this](Entity* entity) {

#define END_ENTITY_BLOCK \
        });

#define START_SINGLE_ENTITY_BLOCK(message) \
        auto current_context = get_current_context(); \
        \
        if (current_context->get_other_entity_count()) {\
            get_current_context()->for_each_other_entities([&,this](Entity* entity) {

#define END_SINGLE_ENTITY_BLOCK(default_value) \
            }); \
        } else { \
            interpreter->set_result(ScriptValue(default_value)); \
        }

void Game::init_other_entity_commands() {
    log::debug("Initializing other entity commands");

#include "_EntityCommands.inl"

    // Custom commands
    m_runtime->register_command(
        "other.pauseOthers",
        [this](Interpreter *interpreter) {
            auto context = get_current_context();

            auto scene = context->get_scene();

            if (scene == nullptr) {
                log::info("{other.pauseOthers} No scene in current context");

                return;
            }

            scene->pause_other_entities(context->get_other_entities());
        },
        {},
        m_runtime->get_void_type(),
        NomadDoc("Pauses all other entities in the scene")
    );
}

} // namespace nomad