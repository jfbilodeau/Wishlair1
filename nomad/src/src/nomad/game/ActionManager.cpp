//
// Created by jfbil on 2023-10-30.
//

#include "nomad/game/ActionManager.hpp"

#include "nomad/game/Game.hpp"
#include "nomad/game/Scene.hpp"

#include "boost/json.hpp"

#include <fstream>

namespace nomad {

ActionManager::ActionManager() {
}

void ActionManager::clear_mapping() {
    m_mappings.clear();
}

NomadId ActionManager::register_action(const NomadString& name, InputCode code1, InputCode code2) {
    auto mapping = get_action_mapping(name);

    if (mapping) {
        mapping->code1 = code1;
        mapping->code2 = code2;

        return mapping->id;
    }

    auto mapping_id = to_nomad_id(m_mappings.size());

    m_mappings.emplace_back(ActionMapping{
        mapping_id,
        name,
        code1,
        code2,
    });

    return mapping_id;
}

bool ActionManager::get_action_name_for_input(InputCode code, NomadString& action_name) const {
    for (auto& mapping : m_mappings) {
        if (mapping.code1 == code || mapping.code2 == code) {
            action_name = mapping.name;
            return true;
        }
    }

    return false;
}

void ActionManager::load_mapping(Game* game, const NomadString& mapping_name) {
    clear_mapping();

    // Load defaults first
    auto default_mapping_file = create_default_mapping_file_name(game, mapping_name);

    load_mapping_from_file(game, default_mapping_file);

    // Load custom if it exists
    auto custom_mapping_file = create_custom_mapping_file_name(game, mapping_name);
    if (game->file_exists(custom_mapping_file)) {
        load_mapping_from_file(game, custom_mapping_file);
    }
}

void ActionManager::save_mapping(Game* game, const NomadString& mapping_name) {
    auto custom_mapping_file = create_custom_mapping_file_name(game, mapping_name);

    save_mapping_to_file(game, custom_mapping_file);
}

void ActionManager::reset_mapping_to_defaults(Game* game, const NomadString& mapping_name) {
    clear_mapping();

    auto custom_mapping_file_name = create_custom_mapping_file_name(game, mapping_name);

    game->delete_file(custom_mapping_file_name);

    load_mapping(game, mapping_name);
}

NomadString ActionManager::create_mapping_file_name(const NomadString& mapping_name) const {
    return mapping_name + ".input.json";
}

NomadString ActionManager::create_default_mapping_file_name(Game* game, const NomadString& mapping_name) const {
    auto file_name = create_mapping_file_name(mapping_name);

    return game->make_resource_path(mapping_name + ".input.json");
}

NomadString ActionManager::create_custom_mapping_file_name(Game* game, const NomadString& mapping_name) const {
    auto file_name = create_mapping_file_name(mapping_name);

    return game->make_settings_path(mapping_name + ".input.json");
}

void ActionManager::load_mapping_from_file(Game* game, const NomadString& file_name) {
    std::ifstream file(file_name);

    if (!file) {
        log::error("Failed to open mapping file for reading: " + file_name);
        return;
    }

    NomadString content(std::istreambuf_iterator<char>(file), {});

    auto json = boost::json::parse(content);

    for (const auto& [key, value] : json.as_object()) {
        NomadString action_name = key;

        auto code1 = get_input_code(value.at("code1").as_string().c_str());
        auto code2 = get_input_code(value.at("code2").as_string().c_str());

        register_action(action_name, code1, code2);
    }
}

void ActionManager::save_mapping_to_file(Game* game, const NomadString& file_name) const {
    std::ofstream file(file_name);

    if (!file) {
        log::error("Failed to open mapping file for writing: " + file_name);
        return;
    }

    boost::json::object json;

    for (const auto& mapping: m_mappings) {
        json[mapping.name] = {
            { "code1", get_input_code_name(mapping.code1) },
            { "code2", get_input_code_name(mapping.code2) },
        };
    }

    file << boost::json::serialize(json);
}

ActionManager::ActionMapping* ActionManager::get_action_mapping(const NomadString& name) {
    for (auto& mapping : m_mappings) {
        if (mapping.name == name) {
            return &mapping;
        }
    }

    return nullptr;
}

} // nomad