//
// Created by jfbil on 2023-10-30.
//

#ifndef NOMAD_ACTIONMANAGER_HPP
#define NOMAD_ACTIONMANAGER_HPP

#include "nomad/nomad.hpp"

#include "nomad/game/InputManager.hpp"

#include <functional>

namespace nomad {

// Forward declarations

class ActionManager {
public:
    explicit ActionManager();
    ActionManager(const ActionManager&) = delete;
    ~ActionManager() = default;

    void clear_mapping();

    NomadId register_action(const NomadString& name, InputCode code1, InputCode code2);

    bool get_action_name_for_input(InputCode code, NomadString& action_name) const;

    void load_mapping(Game* game, const NomadString& mapping_name);
    void save_mapping(Game* game, const NomadString& mapping_name);
    void reset_mapping_to_defaults(Game* game, const NomadString& mapping_name);

private:
    struct ActionMapping {
        NomadId id;
        NomadString name;
        InputCode code1;
        InputCode code2;
    };

    [[nodiscard]] NomadString create_mapping_file_name(const NomadString& mapping_name) const;
    [[nodiscard]] NomadString create_default_mapping_file_name(Game* game, const NomadString& mapping_name) const;
    [[nodiscard]] NomadString create_custom_mapping_file_name(Game* game, const NomadString& mapping_name) const;
    void load_mapping_from_file(Game* game, const NomadString& file_name);
    void save_mapping_to_file(Game* game, const NomadString& file_name) const;

    ActionMapping* get_action_mapping(const NomadString& name);

    std::vector<ActionMapping> m_mappings;
};

} // nomad

#endif //NOMAD_ACTIONMANAGER_HPP
