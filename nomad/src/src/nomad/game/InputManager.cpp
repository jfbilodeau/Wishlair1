//
// Created by jfbil on 2023-06-27.
//

#include "nomad/game/InputManager.hpp"

#include "nomad/game/Game.hpp"

#include "boost/json.hpp"

#include <fstream>

namespace nomad {

std::pair<InputCode, const char*> test = { short(1), "" };

static const std::array<std::pair<InputCode, const char*>, 26> INPUT_NAMES = {{
    // Mouse input names
    { INPUT_MOUSE_BUTTON_LEFT, "Mouse Left" },
    { INPUT_MOUSE_BUTTON_MIDDLE, "Mouse Middle" },
    { INPUT_MOUSE_BUTTON_RIGHT, "Mouse Right" },
    { INPUT_MOUSE_BUTTON_X1, "Mouse X1" },
    { INPUT_MOUSE_BUTTON_X2, "Mouse X2" },

    // Controller input names
    { CONTROLLER1_BUTTON_A, "Controller 1 A" },
    { CONTROLLER1_BUTTON_B, "Controller 1 B" },
    { CONTROLLER1_BUTTON_X, "Controller 1 X" },
    { CONTROLLER1_BUTTON_Y, "Controller 1 Y" },
    { CONTROLLER1_BUTTON_BACK, "Controller 1 Back" },
    { CONTROLLER1_BUTTON_GUIDE, "Controller 1 Guide" },
    { CONTROLLER1_BUTTON_START, "Controller 1 Start" },
    { CONTROLLER1_BUTTON_LEFT_STICK, "Controller 1 Left Stick" },
    { CONTROLLER1_BUTTON_RIGHT_STICK, "Controller 1 Right Stick" },
    { CONTROLLER1_BUTTON_LEFT_SHOULDER, "Controller 1 Left Shoulder" },
    { CONTROLLER1_BUTTON_RIGHT_SHOULDER, "Controller 1 Right Shoulder" },
    { CONTROLLER1_BUTTON_DPAD_UP, "Controller 1 D-Pad Up" },
    { CONTROLLER1_BUTTON_DPAD_DOWN, "Controller 1 D-Pad Down" },
    { CONTROLLER1_BUTTON_DPAD_LEFT, "Controller 1 D-Pad Left" },
    { CONTROLLER1_BUTTON_DPAD_RIGHT, "Controller 1 D-Pad Right" },
    { CONTROLLER1_BUTTON_MISC1, "Controller 1 Misc 1" },
    { CONTROLLER1_BUTTON_PADDLE1, "Controller 1 Paddle 1" },
    { CONTROLLER1_BUTTON_PADDLE2, "Controller 1 Paddle 2" },
    { CONTROLLER1_BUTTON_PADDLE3, "Controller 1 Paddle 3" },
    { CONTROLLER1_BUTTON_PADDLE4, "Controller 1 Paddle 4" },
    { CONTROLLER1_BUTTON_TOUCHPAD, "Controller 1 Touchpad" },
}};

NomadString get_input_code_name(InputCode code) {
    // Look for input name
    for (const auto& input_name: INPUT_NAMES) {
        if (input_name.first == code) {
            return input_name.second;
        }
    }

    // Let's ask SDL for the name
    auto sdl_name = SDL_GetScancodeName(SDL_Scancode(code));

    return sdl_name;
}

InputCode get_input_code(const NomadString& name) {
    // Look for input name
    for (const auto& input_name: INPUT_NAMES) {
        if (input_name.second == name) {
            return input_name.first;
        }
    }

    // Let's ask SDL for the code
    auto sdl_code = SDL_GetScancodeFromName(name.c_str());

    return InputCode(sdl_code);
}

void map_sdl_keyboard_event(SDL_KeyboardEvent& event, InputEvent& input_event) {
    input_event.source = InputSource::Keyboard;
    input_event.action = event.type == SDL_KEYDOWN ? InputAction::Pressed : InputAction::Released;
    input_event.code = InputCode(event.keysym.scancode + INPUT_KEYBOARD_OFFSET);
}

void map_sdl_mouse_event(SDL_MouseButtonEvent& event, InputEvent& input_event) {
    input_event.source = InputSource::Mouse;
    input_event.action = event.type == SDL_MOUSEBUTTONDOWN ? InputAction::Pressed : InputAction::Released;
    input_event.code = InputCode(event.button + INPUT_MOUSE_OFFSET);
}

void map_sdl_controller_button_event(SDL_ControllerButtonEvent& event, InputEvent& input_event) {
    input_event.source = InputSource::Controller;
    input_event.action = event.type == SDL_CONTROLLERBUTTONDOWN ? InputAction::Pressed : InputAction::Released;
    input_event.code = InputCode(event.button + INPUT_CONTROLLER1_OFFSET);
}

//InputManager::InputManager(Game* game):
//    m_game(game)
//{
//}
//
//Game* InputManager::get_game() {
//    return m_game;
//}
//
//void InputManager::clear_all_actions() {
//    m_inputs.clear();
//}
//
//NomadId InputManager::register_action(const NomadString& name, InputCode code1, InputCode code2) {
//    auto input_id = get_input_id(name);
//
//    if (input_id != NOMAD_INVALID_ID) {
//        m_game->get_logger()->warning("Input already registered: " + name);
//
//        return input_id;
//    }
//
//    input_id = to_nomad_id(m_inputs.size());
//
//    m_inputs.push_back({
//        input_id,
//        name,
//        code1,
//        code2,
//    });
//
//    return input_id;
//}
//
//void InputManager::set_code_for_action(const NomadString& action_name, ActionCodeSlot slot, InputCode input_code) {
//    if (slot == ActionCodeSlot::Slot1) {
//        for (auto& input: m_inputs) {
//            if (input.name == action_name) {
//                input.code1 = input_code;
//            }
//        }
//    } else if (slot == ActionCodeSlot::Slot2) {
//        for (auto& input: m_inputs) {
//            if (input.name == action_name) {
//                input.code2 = input_code;
//            }
//        }
//    }
//}
//
//void InputManager::on_start_frame() {
//
//}
//
//void InputManager::on_end_frame() {
//    for (auto& input: m_inputs) {
//        input.pressed = false;
//        input.released = false;
//    }
//}
//
//void InputManager::save_mapping(const NomadString& mapping_name) {
//    auto settings_path = m_game->get_settings_path();
//
//    auto mapping_file = settings_path + mapping_name + ".input.json";
//
//    std::ofstream file(mapping_file);
//
//    if (!file) {
//        m_game->get_logger()->error("Failed to open mapping file for reading: " + mapping_file);
//        return;
//    }
//
//    boost::json::object json;
//
//    for (const auto& input: m_inputs) {
//        json[input.name] = {
//            { "code1", get_input_code_name(input.code1) },
//            { "code2", get_input_code_name(input.code2) },
//        };
//    }
//
//    file << boost::json::serialize(json);
//}
//
//void InputManager::load_mapping(const NomadString& mapping_name) {
//    auto settings_path = m_game->get_settings_path();
//
//    auto mapping_file = settings_path + mapping_name + ".input.json";
//
//    std::ifstream file(mapping_file);
//
//    if (!file) {
//        m_game->get_logger()->error("Failed to open mapping file for writing: " + mapping_file);
//        return;
//    }
//
//    boost::json::parser parser;
//
//    do {
//        char buffer[4096];
//
//        auto read_size = file.readsome(buffer, sizeof(buffer));
//
//        parser.write(buffer, read_size);
//    } while (!file.eof());
//
//    auto json = parser.release();
//
//    auto object = json.as_object();
//
//    for (auto& action: object) {
//        auto action_name = NomadString(action.key());
//        auto action_object = action.value().as_object();
//
//        auto code1_name = NomadString(action_object["code1"].as_string());
//        auto code2_name = NomadString(action_object["code2"].as_string());
//
//        auto code1 = get_input_code(code1_name);
//        auto code2 = get_input_code(code2_name);
//
//        register_action(action_name, code1, code2);
//    }
//}
//
//void InputManager::update_input(SDL_KeyboardEvent& event) {
//    auto code = InputCode(event.keysym.scancode + INPUT_KEYBOARD_OFFSET);
//
//    for (auto& input: m_inputs) {
//        if (input.code1 == code || input.code2 == code) {
//            if (event.type == SDL_KEYDOWN) {
//                input.pressed = true;
//                input.held = true;
//            } else if (event.type == SDL_KEYUP) {
//                input.released = true;
//                input.held = false;
//            }
//        }
//    }
//}
//
//void InputManager::update_input(SDL_MouseButtonEvent& event) {
//    int code = event.button + INPUT_MOUSE_OFFSET;
//
//    for (auto& input: m_inputs) {
//        if (input.code1 == code || input.code2) {
//            if (event.type == SDL_MOUSEBUTTONDOWN) {
//                input.pressed = true;
//                input.held = true;
//            } else if (event.type == SDL_MOUSEBUTTONUP) {
//                input.released = true;
//                input.held = false;
//            }
//        }
//    }
//}
//
//void InputManager::update_input(SDL_ControllerButtonEvent& event) {
//    int code = event.button + INPUT_CONTROLLER1_OFFSET;
//
//    for (auto& input: m_inputs) {
//        if (input.code1 == code || input.code2 == code) {
//            if (event.type == SDL_CONTROLLERBUTTONDOWN) {
//                input.pressed = true;
//                input.held = true;
//            } else if (event.type == SDL_CONTROLLERBUTTONUP) {
//                input.released = true;
//                input.held = false;
//            }
//        }
//    }
//}
//
//NomadId InputManager::get_input_id(const NomadString& name) const {
//    for (const auto& input: m_inputs) {
//        if (input.name == name) {
//            return input.id;
//        }
//    }
//
//    return NOMAD_INVALID_ID;
//}
//
//const Input& InputManager::get_input(NomadId id) const {
//    return m_inputs[id];
//}
//
//bool InputManager::is_pressed(NomadId id) const {
//    return m_inputs[id].pressed;
//}
//
//bool InputManager::is_released(NomadId id) const {
//    return m_inputs[id].released;
//}
//
//bool InputManager::is_held(NomadId id) const {
//    return m_inputs[id].held;
//}
//
//bool InputManager::is_pressed(const NomadString& action_name) const {
//    for (const auto& input: m_inputs) {
//        if (input.name == action_name) {
//            return input.pressed;
//        }
//    }
//
//    return false;
//}
//
//bool InputManager::is_released(const NomadString& action_name) const {
//    for (const auto& input: m_inputs) {
//        if (input.name == action_name) {
//            return input.released;
//        }
//    }
//
//    return false;
//}
//
//bool InputManager::is_held(const NomadString& action_name) const {
//    for (const auto& input: m_inputs) {
//        if (input.name == action_name) {
//            return input.held;
//        }
//    }
//
//    return false;
//}

} // nomad