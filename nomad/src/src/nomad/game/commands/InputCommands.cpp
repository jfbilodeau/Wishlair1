//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Entity.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Runtime.hpp"

namespace nomad {

void Game::initInputCommands() {
    log::debug("Initializing input commands");

//    m_runtime->register_command(
//        "input.register",
//        [this](Interpreter* interpreter) {
//            auto action = interpreter->execute_string_expression();
//            auto input1 = interpreter->execute_expression();
//            auto input2 = interpreter->execute_expression();
//
//            m_input_manager.register_action(action);
//        }, {
//            def_parameter(CommandParameterType::String NomadParamDoc("Name of the action to register.")),
//            def_parameter(CommandParameterType::Number NomadParamDoc("First input to map to the action.")),
//            def_parameter(CommandParameterType::Number NomadParamDoc("Second input to map to the action.")),
//        },
//        false
//        NomadDoc("Registers a new input action.")
//    );
//
//    m_runtime->register_command(
//        "input.pressed",
//        [this](Interpreter* interpreter) {
//            auto action = interpreter->execute_string_expression();
//
//            interpreter->set_result(m_input_manager.is_pressed(action));
//        }, {
//            def_parameter(CommandParameterType::String NomadParamDoc("Name of the action to check."))
//        },
//        true
//        NomadDoc("Returns true if the action has been press in this frame pressed.")
//    );
//
//    m_runtime->register_command(
//        "input.released",
//        [this](Interpreter* interpreter) {
//            auto action = interpreter->execute_string_expression();
//
//            interpreter->set_result(m_input_manager.is_released(action));
//        }, {
//            def_parameter(CommandParameterType::String NomadParamDoc("Name of the action to check."))
//        },
//        true
//        NomadDoc("Returns true if the action has been released in this frame.")
//    );
//
//    m_runtime->register_command(
//        "input.held",
//        [this](Interpreter* interpreter) {
//            auto action = interpreter->execute_string_expression();
//
//            interpreter->set_result(m_input_manager.is_held(action));
//        }, {
//            def_parameter(CommandParameterType::String NomadParamDoc("Name of the action to check."))
//        },
//        true
//        NomadDoc("Returns true if the action is currently held.")
//    );
}

} // nomad