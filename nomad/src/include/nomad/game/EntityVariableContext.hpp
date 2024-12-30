//
// Created by jfbil on 2023-07-18.
//

#ifndef NOMAD_ENTITYVARIABLECONTEXT_HPP
#define NOMAD_ENTITYVARIABLECONTEXT_HPP

#include "nomad/script/VariableContext.hpp"

namespace nomad {

// Forward declarations
class Game;

class EntityVariableContext : public SimpleVariableContext {
public:
    explicit EntityVariableContext(Game* game);
    EntityVariableContext(const EntityVariableContext&) = delete;
    ~EntityVariableContext() override = default;

    void set_value(NomadId variable_id, const ScriptValue& value) override;
    void get_value(NomadId variable_id, ScriptValue& value) override;

private:
    Game* m_game;
};

} // nomad

#endif //NOMAD_ENTITYVARIABLECONTEXT_HPP
