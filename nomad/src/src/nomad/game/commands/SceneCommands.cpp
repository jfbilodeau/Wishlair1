//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/system/TempHeap.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define CHECK_SCENE_NOT_NULL(message) \
    auto scene = getCurrentContext()->getScene(); \
    if (scene == nullptr) { \
        log::error(message); \
        return; \
    }

void Game::initSceneCommands() {
    log::debug("Initializing scene commands");

    m_runtime->registerCommand(
        "scene.camera.follow",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot set camera position outside of a scene")

            auto entityId = interpreter->getIdParameter(0);

            scene->cameraStartFollowEntity(entityId);
        }, {
            defParameter("entityId", m_runtime->getIntegerType(), NomadParamDoc("ID of the entity to follow."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Makes the camera follow the specified entity.")
    );

    m_runtime->registerCommand(
        "scene.camera.setPosition",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot set camera position outside of a scene")

            auto x = interpreter->getFloatParameter(0);
            auto y = interpreter->getFloatParameter(1);

            scene->setCameraPosition(x, y);
        }, {
            defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X position of the camera.")),
            defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y position of the camera."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Sets the camera position.")
    );

    m_runtime->registerCommand(
        "scene.createEntity",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot create entity outside of a scene")

            auto initScriptName = interpreter->getStringParameter(0);
            const auto entityX = interpreter->getFloatParameter(1);
            const auto entityY = interpreter->getFloatParameter(2);
            const auto layer = interpreter->getIntegerParameter(3);


            scene->createEntity(initScriptName, entityX, entityY, layer);
        }, {
            defParameter(
                "scriptName", m_runtime->getStringType(),
                NomadParamDoc("Name of the script to execute to initialize the entity.")
            ),
            defParameter("x", m_runtime->getFloatType(), NomadParamDoc("X position of the entity.")),
            defParameter("y", m_runtime->getFloatType(), NomadParamDoc("Y position of the entity.")),
            defParameter("layer", m_runtime->getIntegerType(), NomadParamDoc("Layer of the entity."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Creates a new entity for this scene.")
    );


    m_runtime->registerCommand(
        "scene.loadInputMapping",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto mappingName = interpreter->getStringParameter(0);

            scene->loadActionMapping(mappingName);
        }, {
            defParameter(
                "mappingName", m_runtime->getStringType(), NomadParamDoc("Name of the input mapping to load.")
            )
        },
        m_runtime->getVoidType(),
        NomadDoc("Load an input mapping for this scene.")
    );

    m_runtime->registerCommand(
        "scene.loadMap",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            NomadString mapName = interpreter->getStringParameter(0);
            NomadString tileSetTexture = interpreter->getStringParameter(1);

            auto mapFileName = mapName + ".tmj";
            auto tileSetTextureFileName = tileSetTexture + ".png";

            scene->loadTileMap(mapFileName, tileSetTextureFileName);
        }, {
            defParameter("mapName", m_runtime->getStringType(), NomadParamDoc("Name of the map to load.")),
            defParameter("tileSetTexture", m_runtime->getStringType(), NomadParamDoc("Name of the tile set texture to load."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Loads a map for this scene.")
    );

    m_runtime->registerCommand(
        "scene.pauseAll",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot pause entities outside of a scene")

            scene->pauseAllEntities();
        },
        {},
        m_runtime->getVoidType(),
        NomadDoc("Pauses all entities in this scene.")
    );

    m_runtime->registerCommand(
        "scene.unpauseAll",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot unpause entities outside of a scene")

            scene->unpauseAllEntities();
        },
        {},
        m_runtime->getVoidType(),
        NomadDoc("Unpauses all entities in this scene.")
    );

    m_runtime->registerCommand(
        "select",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto predicateId = interpreter->getIdParameter(0);

            auto executionContext = getCurrentContext();
            auto thisEntity = executionContext->getThisEntity();

            auto otherEntities = createTempVector<Entity*>();

            NomadInteger layerIndex = thisEntity->getLayer();

            scene->forEachEntityByLayer(
                layerIndex,
                [&](Entity* entity) {
                    if (entity != thisEntity) {
                        executionContext->clearOtherEntitiesAndAdd(entity);

                        auto result = executePredicate(predicateId);

                        if (result) {
                            otherEntities.push_back(entity);
                        }
                    }
                }
            );

            executionContext->setOtherEntities(otherEntities);
        }, {
            defParameter(
                "predicate", m_runtime->getPredicateType(),
                NomadParamDoc("The predicate used to select other entities")
            ),
        },
        m_runtime->getIntegerType(),
        NomadDoc("Select entities in the same layer as the `this` entity that match the predicate.")
    );

    m_runtime->registerCommand(
        "select.all",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto predicateId = interpreter->getIdParameter(0);

            auto executionContext = getCurrentContext();
            auto thisEntity = executionContext->getThisEntity();

            auto otherEntities = createTempVector<Entity*>();

            executionContext->clearOtherEntities();

            scene->forEachEntities(
                [&](Entity* entity) {
                    if (entity != thisEntity) {
                        executionContext->clearOtherEntitiesAndAdd(entity);

                        auto result = executePredicate(predicateId);

                        if (result) {
                            otherEntities.push_back(entity);
                        }
                    }
                }
            );

            executionContext->setOtherEntities(otherEntities);
        }, {
            defParameter(
                "predicate", m_runtime->getPredicateType(),
                NomadParamDoc("The predicate used to select other entities")
            ),
        },
        m_runtime->getIntegerType(),
        NomadDoc("Select entities in all layers that match the predicate.")
    );

    m_runtime->registerCommand(
        "select.byName",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto name = interpreter->getStringParameter(0);

            auto executionContext = getCurrentContext();
            auto thisEntity = executionContext->getThisEntity();

            auto layerIndex = thisEntity->getLayer();

            executionContext->clearOtherEntities();

            scene->forEachEntityByLayer(
                layerIndex,
                [&](Entity* entity) {
                    if (entity->getName() == name) {
                        executionContext->addOtherEntity(entity);
                    }
                }
            );
        }, {
            defParameter("name", m_runtime->getStringType(), NomadParamDoc("The name of the entity to select")),
        },
        m_runtime->getIntegerType(),
        NomadDoc("Select entities in the same layer as the `this` entity that have the given name.")
    );

    m_runtime->registerCommand(
        "select.all.byName",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto name = interpreter->getStringParameter(0);

            auto executionContext = getCurrentContext();

            executionContext->clearOtherEntities();

            scene->forEachEntities(
                [&](Entity* entity) {
                    if (entity->getName() == name) {
                        executionContext->addOtherEntity(entity);
                    }
                }
            );
        }, {
            defParameter("name", m_runtime->getStringType(), NomadParamDoc("The name of the entity to select")),
        },
        m_runtime->getIntegerType(),
        NomadDoc("Select entities across all layers that have the given name.")
    );

    m_runtime->registerCommand(
        "select.this",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto executionContext = getCurrentContext();
            auto thisEntity = executionContext->getThisEntity();

            executionContext->clearOtherEntities();
            executionContext->addOtherEntity(thisEntity);
        },
        { },
        m_runtime->getVoidType(),
        NomadDoc("Select the `this` as `other` entity. Useful to pass the `this` entity to a function that takes `other` entities.")
    );

    m_runtime->registerCommand(
        "scene.events.trigger.layer",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot trigger event outside of a scene")

            auto eventName = interpreter->getStringParameter(0);
            auto layerId = interpreter->getIntegerParameter(1);

            scene->triggerEventLayer(eventName, layerId);
        }, {
            defParameter("eventName", m_runtime->getStringType(), NomadParamDoc("Name of the event to trigger.")),
            defParameter("layerId", m_runtime->getIntegerType(), NomadParamDoc("Layer ID to trigger the event on."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Trigger an event for all entities on the specified layer.")
    );

    m_runtime->registerCommand(
        "scene.events.trigger",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot trigger event outside of a scene")

            auto eventName = interpreter->getStringParameter(0);

            scene->scheduleEvent(eventName, 0);
        }, {
            defParameter("eventName", m_runtime->getStringType(), NomadParamDoc("Name of the event to trigger."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Trigger an event for all entities on this scene.")
    );

    m_runtime->registerCommand(
        "scene.events.queue",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot trigger event outside of a scene")

            auto eventName = interpreter->getStringParameter(1);
            auto frameCount = interpreter->getIntegerParameter(0);

            scene->scheduleEvent(eventName, frameCount);
        },
        {
            defParameter("frameCount", m_runtime->getIntegerType(), NomadParamDoc("Number of frames to wait before triggering the event.")),
            defParameter("eventName", m_runtime->getStringType(), NomadParamDoc("Name of the event to trigger."))
        },
        m_runtime->getVoidType(),
        NomadDoc("Trigger an event after a specified number of frames.")
    );
}

} // nomad