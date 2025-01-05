//
// Created by jfbil on 2023-06-25.
//

#ifndef NOMAD_GAMEEXECUTIONCONTEXT_HPP
#define NOMAD_GAMEEXECUTIONCONTEXT_HPP

#include "nomad/nomad.hpp"

#include <functional>

namespace nomad {

// Forward declarations
class Entity;
class Game;
class Scene;

class GameExecutionContext {
public:
    explicit GameExecutionContext() = default;
    GameExecutionContext(const GameExecutionContext&) = delete;
    ~GameExecutionContext() = default;

    void reset(Scene* scene, Entity* this_entity);

    void set_scene(Scene* scene);
    [[nodiscard]] Scene* get_scene() const;

    void set_this_entity(Entity* entity);
    [[nodiscard]] Entity* get_this_entity() const;

    void push_other_entity(Entity* entity);
    [[nodiscard]] NomadIndex get_other_entity_count() const;
    void clear_other_entities();
    void add_other_entity(Entity* entity);
    void clear_other_entities_and_add(Entity* entity);
    void set_other_entities(const std::vector<Entity*>& entities);

    void for_each_other_entities(const std::function<void(Entity*)>& callback) {
        for (auto& entity : m_other_entities) {
            callback(entity);
        }
    }

private:
    Scene* m_scene = nullptr;
    Entity* m_entity = nullptr;
    std::vector<Entity*> m_other_entities;
};

} // nomad

#endif //NOMAD_GAMEEXECUTIONCONTEXT_HPP
