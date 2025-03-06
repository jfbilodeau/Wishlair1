//
// Created by jfbil on 2024-12-06.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"
#include "nomad/resource/Sprite.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define VARIABLE_NAME_PREFIX "other"

#define START_ENTITY_BLOCK(message) \
    get_current_context()->for_each_other_entities([&,this](Entity* entity) {

#define END_ENTITY_BLOCK \
    });

#define START_SINGLE_ENTITY_BLOCK(message) \
    auto current_context = get_current_context(); \
    if (current_context->get_other_entity_count()) {\
        get_current_context()->for_each_other_entities([&,this](Entity* entity) {

#define END_SINGLE_ENTITY_BLOCK(default_value) \
        }); \
    } else { \
        value.set(default_value); \
    }

void Game::init_other_dynamic_variables() {
    log::debug("Initializing this dynamic variables");

    m_runtime->register_dynamic_variable(
        "other",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            START_SINGLE_ENTITY_BLOCK("Cannot access 'other' outside of an entity")

            value.set_integer_value(entity->get_id());

            END_SINGLE_ENTITY_BLOCK(static_cast<NomadInteger>(NOMAD_INVALID_ID))
        },
        m_runtime->get_integer_type(),
        "Get the ID of the `other` entity."
    );

    m_runtime->register_dynamic_variable(
        "others.count",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            log::error("Cannot set 'other.count'");
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.set_integer_value(get_current_context()->get_other_entity_count());
        },
        m_runtime->get_integer_type(),
        NomadDoc("Returns the number of selected 'other' entities")
    );

    #include "_EntityDynamicVariables.inl"
}

} // namespace nomad