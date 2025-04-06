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
        getCurrentContext()->forEachOtherEntities([&,this](Entity* entity) {

#define END_ENTITY_BLOCK \
        });

#define START_SINGLE_ENTITY_BLOCK(message) \
        auto currentContext = getCurrentContext(); \
        \
        if (currentContext->getOtherEntityCount()) {\
            currentContext->forEachOtherEntities([&,this](Entity* entity) {

#define END_SINGLE_ENTITY_BLOCK(defaultValue) \
            }); \
        } else { \
            interpreter->setResult(ScriptValue(defaultValue)); \
        }

void Game::initOtherEntityCommands() {
    log::debug("Initializing other entity commands");

#include "_EntityCommands.inl"

    // Custom commands
    m_runtime->registerCommand(
        "other.pauseOthers",
        [this](Interpreter *interpreter) {
            auto context = getCurrentContext();

            auto scene = context->getScene();

            if (scene == nullptr) {
                log::info("{other.pauseOthers} No scene in current context");

                return;
            }

            scene->pauseOtherEntities(context->getOtherEntities());
        },
        {},
        m_runtime->getVoidType(),
        NomadDoc("Pauses all other entities in the scene")
    );
}

} // namespace nomad