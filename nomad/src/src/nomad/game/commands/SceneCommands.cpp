//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/system/FastHeap.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

#define CHECK_SCENE_NOT_NULL(message) \
    if (get_current_context()->get_scene() == nullptr) { \
        log::error(message); \
        return; \
    }

void Game::init_scene_commands() {
    log::debug("Initializing scene commands");

    m_runtime->register_command(
        "scene.loadInputMapping",
        [this](Interpreter* interpreter) {
            CHECK_SCENE_NOT_NULL("Cannot load input mapping outside of a scene")

            auto mapping_name = interpreter->get_string_parameter(0);

            auto scene = get_current_context()->get_scene();

            scene->load_action_mapping(mapping_name);
        }, {
            def_parameter(
                "mappingName", m_runtime->get_string_type(), NomadParamDoc("Name of the input mapping to load.")
            )
        },
        m_runtime->get_void_type(),
        NomadDoc("Load an input mapping for this scene.")
    );

    m_runtime->register_command(
        "scene.createEntity",
        [this](
            Interpreter* interpreter
        ) {
            CHECK_SCENE_NOT_NULL("Cannot create entity outside of a scene")

            auto init_script_name = interpreter->get_string_parameter(0);
            const auto entity_x = interpreter->get_float_parameter(1);
            const auto entity_y = interpreter->get_float_parameter(2);
            const auto layer = interpreter->get_integer_parameter(3);

            const auto scene = get_current_context()->get_scene();

            scene->create_entity(init_script_name, entity_x, entity_y, layer);
        }, {
            def_parameter(
                "scriptName", m_runtime->get_string_type(),
                NomadParamDoc("Name of the script to execute to initialize the entity.")
            ),
            def_parameter("x", m_runtime->get_float_type(), NomadParamDoc("X position of the entity.")),
            def_parameter("y", m_runtime->get_float_type(), NomadParamDoc("Y position of the entity.")),
            def_parameter("layer", m_runtime->get_integer_type(), NomadParamDoc("Layer of the entity."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Creates a new entity for this scene.")
    );


    m_runtime->register_command(
        "scene.loadMap",
        [this](Interpreter* interpreter) {
            NomadString map_name = interpreter->get_string_parameter(0);
            NomadString tile_set_texture = interpreter->get_string_parameter(1);

            auto map_file_name = map_name + ".tmj";
            auto tile_set_texture_file_name = tile_set_texture + ".png";

            auto scene = get_current_context()->get_scene();

            scene->load_tile_map(map_file_name, tile_set_texture_file_name);
        }, {
            def_parameter("mapName", m_runtime->get_string_type(), NomadParamDoc("Name of the map to load.")),
            def_parameter("tileSetTexture", m_runtime->get_string_type(), NomadParamDoc("Name of the tile set texture to load."))
        },
        m_runtime->get_void_type(),
        NomadDoc("Loads a map for this scene.")
    );

    m_runtime->register_command(
        "select",
        [this](Interpreter* interpreter) {
            auto predicate_id = interpreter->get_id_parameter(0);

            auto execution_context = get_current_context();
            auto this_entity = execution_context->get_this_entity();
            auto scene = execution_context->get_scene();

            TempVector<Entity*> other_entities(&fast_heap_allocator);

            NomadInteger layer_index = this_entity->get_layer();

            scene->for_each_entity_by_layer(
                layer_index,
                [&](Entity* entity) {
                    if (entity != this_entity) {
                        execution_context->clear_other_entities_and_add(entity);

                        auto result = execute_predicate(predicate_id);

                        if (result) {
                            other_entities.push_back(entity);
                        }
                    }
                }
            );

            execution_context->set_other_entities(other_entities);
        }, {
            def_parameter(
                "predicate", m_runtime->get_predicate_type(),
                NomadParamDoc("The predicate used to select other entities")
            ),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Select entities in the same layer as the `this` entity that match the predicate.")
    );

    m_runtime->register_command(
        "select.all",
        [this](Interpreter* interpreter) {
            auto predicate_id = interpreter->get_id_parameter(0);

            auto execution_context = get_current_context();
            auto this_entity = execution_context->get_this_entity();
            auto scene = execution_context->get_scene();

            TempVector<Entity*> other_entities;

            execution_context->clear_other_entities();

            scene->for_each_entities(
                [&](Entity* entity) {
                    if (entity != this_entity) {
                        execution_context->clear_other_entities_and_add(entity);

                        auto result = execute_predicate(predicate_id);

                        if (result) {
                            other_entities.push_back(entity);
                        }
                    }
                }
            );

            execution_context->set_other_entities(other_entities);
        }, {
            def_parameter(
                "predicate", m_runtime->get_predicate_type(),
                NomadParamDoc("The predicate used to select other entities")
            ),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Select entities in all layers that match the predicate.")
    );

    m_runtime->register_command(
        "select.byName",
        [this](Interpreter* interpreter) {
            auto name = interpreter->get_string_parameter(0);

            auto execution_context = get_current_context();
            auto this_entity = execution_context->get_this_entity();
            auto scene = execution_context->get_scene();

            // Use a static variable to avoid reallocating storage for the vector
            TempVector<Entity*> other_entities;

            NomadInteger layer_index = this_entity->get_layer();

            execution_context->clear_other_entities();

            scene->for_each_entity_by_layer(
                layer_index,
                [&](Entity* entity) {
                    if (entity != this_entity && entity->get_name() == name) {
                        execution_context->add_other_entity(entity);
                    }
                }
            );
        }, {
            def_parameter("name", m_runtime->get_string_type(), NomadParamDoc("The name of the entity to select")),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Select entities in the same layer as the `this` entity that have the given name.")
    );

    m_runtime->register_command(
        "select.all.byName",
        [this](Interpreter* interpreter) {
            auto name = interpreter->get_string_parameter(0);

            auto execution_context = get_current_context();
            auto this_entity = execution_context->get_this_entity();
            auto scene = execution_context->get_scene();

            // Use a static variable to avoid reallocating storage for the vector
            TempVector<Entity*> other_entities;

            execution_context->clear_other_entities();

            scene->for_each_entities(
                [&](Entity* entity) {
                    if (entity != this_entity && entity->get_name() == name) {
                        execution_context->add_other_entity(entity);
                    }
                }
            );
        }, {
            def_parameter("name", m_runtime->get_string_type(), NomadParamDoc("The name of the entity to select")),
        },
        m_runtime->get_integer_type(),
        NomadDoc("Select entities across all layers that have the given name.")
    );
}

} // nomad