//
// Created by jfbil on 2023-09-20.
//

#ifndef NOMAD_DEBUGCONSOLE_HPP
#define NOMAD_DEBUGCONSOLE_HPP

#include "nomad/nomad.hpp"

#include "nomad/log/MemorySink.hpp"

#include "imgui.h"

#include "SDL.h"

#include <vector>

namespace nomad {

class Game;
class Script;

class DebugConsole {
public:
    explicit DebugConsole(Game* game);
    ~DebugConsole();

    void activate();
    void deactivate();

    [[nodiscard]] bool is_active() const;

    void process_event(const SDL_Event& event);
    void render();

private:
    struct ScriptInfo {
        Script* script;
        NomadString formatted_source;
        NomadString decompiled_source;
    };

private:
    Game* m_game;
    MemorySink m_memory_sink;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    ImGuiIO* m_io;
    bool m_window_focus = false;
    bool m_active = false;
    bool m_minimized = false;
    bool m_dockspace_first_time = true;
    std::vector<ScriptInfo> m_scripts;
    ScriptInfo* m_selected_script = nullptr;
};

} // nomad

#endif //NOMAD_DEBUGCONSOLE_HPP
