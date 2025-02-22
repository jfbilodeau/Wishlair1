//
// Created by jfbil on 2023-07-18.
//

#ifndef NOMAD_SCENEVARIABLECONTEXT_HPP
#define NOMAD_SCENEVARIABLECONTEXT_HPP

#include "nomad/script/VariableContext.hpp"

namespace nomad {

// Forward declarations
class Game;

class SceneVariableContext : public VariableContext {
public:
    explicit SceneVariableContext(Game* game);
    SceneVariableContext(const SceneVariableContext&) = delete;
    ~SceneVariableContext() override = default;

    void set_value(NomadId variable_id, const ScriptValue& value) override;
    void get_value(NomadId variable_id, ScriptValue& value) override;

private:
    Game* m_game;
};

} // nomad

#endif // #ifndef NOMAD_SCENEVARIABLECONTEXT_HPP
