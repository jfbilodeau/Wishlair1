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
    auto entity = getCurrentContext()->getThisEntity(); \
    if (entity == nullptr) { \
        log::error(message);  \
        return; \
    }

#define END_ENTITY_BLOCK

#define START_SINGLE_ENTITY_BLOCK(message) \
    START_ENTITY_BLOCK(message)

#define END_SINGLE_ENTITY_BLOCK(default_value)

void Game::initThisEntityCommands() {
    log::debug("Initializing this entity commands");

    #include "_EntityCommands.inl"

    // Custom commands
    m_runtime->registerCommand(
        "this.pauseOthers",
        [this](Interpreter *interpreter) {
            auto context = getCurrentContext();

            auto entity = context->getThisEntity();
            auto scene = context->getScene();

            if (entity == nullptr) {
                log::info("{this.pauseOthers}: No entity in current context");

                return;
            }

            if (scene == nullptr) {
                log::info("{this.pauseOthers} No scene in current context");

                return;
            }

            scene->pauseOtherEntities(entity);
        },
        {},
        m_runtime->getVoidType(),
        NomadDoc("Pauses all other entities in the scene except `this`.")
    );
}

} // namespace nomad