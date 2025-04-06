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
    getCurrentContext()->forEachOtherEntities([&,this](Entity* entity) {

#define END_ENTITY_BLOCK \
    });

#define START_SINGLE_ENTITY_BLOCK(message) \
    auto currentContext = getCurrentContext(); \
    auto entity = currentContext->getFirstOtherEntity(); \
    if (entity) {\

#define END_SINGLE_ENTITY_BLOCK(defaultValue) \
    } else { \
        value.set(defaultValue); \
    }

void Game::initOtherDynamicVariables() {
    log::debug("Initializing this dynamic variables");

    m_runtime->registerDynamicVariable(
        "other",
        nullptr,
        [this](Interpreter* interpreter, ScriptValue& value) {
            START_SINGLE_ENTITY_BLOCK("Cannot access 'other' outside of an entity")

            value.setIntegerValue(entity->getId());

            END_SINGLE_ENTITY_BLOCK(static_cast<NomadInteger>(NOMAD_INVALID_ID))
        },
        m_runtime->getIntegerType(),
        "Get the ID of the `other` entity."
    );

    m_runtime->registerDynamicVariable(
        "others.count",
        [this](Interpreter* interpreter, const ScriptValue& value) {
            log::error("Cannot set 'other.count'");
        },
        [this](Interpreter* interpreter, ScriptValue& value) {
            value.setIntegerValue(getCurrentContext()->getOtherEntityCount());
        },
        m_runtime->getIntegerType(),
        NomadDoc("Returns the number of selected 'other' entities")
    );

    #include "_EntityDynamicVariables.inl"
}

} // namespace nomad