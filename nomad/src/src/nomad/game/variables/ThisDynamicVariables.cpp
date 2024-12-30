//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"
#include "nomad/resource/Sprite.hpp"

#include "nomad/script/Interpreter.hpp"
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

void Game::init_this_dynamic_variables() {
    log::debug("Initializing this dynamic variables");

    #include "_EntityDynamicVariables.inl"
}

} // namespace nomad