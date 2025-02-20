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

    m_runtime->register_dynamic_variable(
        "this",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            START_ENTITY_BLOCK("Cannot access 'this' outside of an entity")

            value.set_integer_value(entity->get_id());

            END_SINGLE_ENTITY_BLOCK(NOMAD_INVALID_ID)
        },
        m_runtime->get_integer_type(),
        "Get the ID of the `this` entity."
    );

    #include "_EntityDynamicVariables.inl"
}

} // namespace nomad