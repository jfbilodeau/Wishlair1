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

#define VARIABLE_NAME_PREFIX "this"

#define START_ENTITY_BLOCK(message) \
    auto entity = get_current_context()->get_this_entity(); \
    if (entity == nullptr) { \
        log::error(message);  \
        return; \
    }

#define END_ENTITY_BLOCK

#define START_SINGLE_ENTITY_BLOCK(message) \
    START_ENTITY_BLOCK(message)

#define END_SINGLE_ENTITY_BLOCK(default_value)

void Game::init_this_entity_commands() {
    log::debug("Initializing this entity commands");

    #include "_EntityCommands.inl"

    // Custom commands
    m_runtime->register_command(
        "this.pauseOthers",
        [this](Interpreter *interpreter) {
            auto context = get_current_context();

            auto entity = context->get_this_entity();
            auto scene = context->get_scene();

            if (entity == nullptr) {
                log::info("{this.pauseOthers}: No entity in current context");

                return;
            }

            if (scene == nullptr) {
                log::info("{this.pauseOthers} No scene in current context");

                return;
            }

            scene->pause_other_entities(entity);
        },
        {},
        m_runtime->get_void_type(),
        NomadDoc("Pauses all other entities in the scene except `this`.")
    );
}

} // namespace nomad