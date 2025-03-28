//
// Created by jfbil on 2023-06-25.
//

#include "nomad/game/GameExecutionContext.hpp"

#include "nomad/system/TempHeap.hpp"

namespace nomad {

void GameExecutionContext::reset(Scene* scene, Entity* entity) {
    m_scene = scene;
    m_entity = entity;
    clear_other_entities();
}

void GameExecutionContext::set_scene(Scene* scene) {
    m_scene = scene;
}

Scene* GameExecutionContext::get_scene() const {
    return m_scene;
}

void GameExecutionContext::set_this_entity(Entity* entity) {
    m_entity = entity;
}

Entity* GameExecutionContext::get_this_entity() const {
    return m_entity;
}

void GameExecutionContext::push_other_entity(Entity* entity) {
    m_other_entities.push_back(entity);
}

NomadIndex GameExecutionContext::get_other_entity_count() const {
    return m_other_entities.size();
}

void GameExecutionContext::clear_other_entities() {
    m_other_entities.clear();
}

void GameExecutionContext::add_other_entity(Entity* entity) {
    m_other_entities.push_back(entity);
}

void GameExecutionContext::clear_other_entities_and_add(Entity* entity) {
    m_other_entities.clear();
    m_other_entities.push_back(entity);
}

void GameExecutionContext::set_other_entities(const std::vector<Entity*>& entities) {
    m_other_entities = entities;
}

void GameExecutionContext::set_other_entities(const TempVector<Entity*>& entities) {
    m_other_entities.assign(entities.begin(), entities.end());
}

const std::vector<Entity*>& GameExecutionContext::get_other_entities() const {
    return m_other_entities;
}

Entity * GameExecutionContext::get_first_other_entity() const {
    if (m_other_entities.empty()) {
        return nullptr;
    }

    return m_other_entities.front();
}
} // nomad