//
// Created by jfbil on 2023-09-20.
//

#include "nomad/debug/DebugConsole.hpp"

#include "nomad/debug/Decompiler.hpp"

#include "nomad/game/Game.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include "nomad/script/Script.hpp"
#include "nomad/script/Runtime.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include <iomanip>
#include <sstream>

namespace nomad {

DebugConsole::DebugConsole(Game* game) :
    m_game(game)
{
    m_game->get_logger()->add_sink(&m_memory_sink);
    m_game->get_logger()->debug("Initializing DebugConsole");

    std::vector<Script*> scripts;
    m_game->get_runtime()->get_scripts(scripts);

    for (auto& script: scripts) {
        ScriptInfo script_info;
        script_info.script = script;

        // Split lines.
        auto source = script->get_source();
        std::istringstream source_stream(source);
        std::string line;

        // Format source with prefix line numbers.
        std::ostringstream line_stream;
        int line_number = 1;

        while (std::getline(source_stream, line)) {
            line_stream << "|" << std::setw(4) << std::setfill('0') << line_number << "|  " << " " << line << std::endl;

            line_number++;
        }

        script_info.formatted_source = line_stream.str();

        m_scripts.push_back(script_info);
    }

    // Sort scripts by name
    std::sort(m_scripts.begin(), m_scripts.end(), [](ScriptInfo& a, ScriptInfo& b) {
        return a.script->get_name() < b.script->get_name();
    });

    m_window = SDL_CreateWindow("Debug Console", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                                SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_ShowWindow(m_window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();

    // Disable INI file
    m_io->IniFilename = nullptr;

//    (void)m_io;
//    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

    m_io->Fonts->AddFontDefault();
}

DebugConsole::~DebugConsole() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);

    m_game->get_logger()->debug("Shutting down DebugConsole");
    m_game->get_logger()->remove_sink(&m_memory_sink);
}

void DebugConsole::activate() {
    if (!m_active) {
        m_active = true;
        m_io->WantCaptureKeyboard = true;
        m_io->WantCaptureMouse = true;

        SDL_ShowWindow(m_window);
        SDL_RaiseWindow(m_window);
    }
}

void DebugConsole::deactivate() {
    if (m_active) {
        m_active = false;
        m_io->WantCaptureKeyboard = false;
        m_io->WantCaptureMouse = false;

        SDL_HideWindow(m_window);
    }
}

bool DebugConsole::is_active() const {
    return m_active;
}

void DebugConsole::process_event(const SDL_Event& event) {
    if (
        event.type == SDL_WINDOWEVENT &&
        event.window.windowID == SDL_GetWindowID(m_window)
        ) {
        switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                deactivate();
                break;

            case SDL_WINDOWEVENT_FOCUS_GAINED:
                m_window_focus = true;
                m_io->WantCaptureKeyboard = true;
                m_io->WantCaptureMouse = true;
                break;

            case SDL_WINDOWEVENT_FOCUS_LOST:
                m_window_focus = false;
                m_io->WantCaptureKeyboard = false;
                m_io->WantCaptureMouse = false;
                break;

            case SDL_WINDOWEVENT_MINIMIZED:
                m_minimized = true;
                break;

            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_MAXIMIZED:
                m_minimized = false;
                break;
        }
    }

    ImGui_ImplSDL2_ProcessEvent(&event);
}

void DebugConsole::render() {
    if (m_minimized) {
        // Skip rendering if minimized
        return;
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Init viewport
    // Ref: https://gist.github.com/PossiblyAShrub/0aea9511b84c34e191eaa90dd7225969
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    auto dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    if (m_dockspace_first_time) {
        m_dockspace_first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, m_io->DisplaySize);

        auto tool_bar_node = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.1f, nullptr, &dockspace_id);
        auto logs_node = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.1f, nullptr, &dockspace_id);
        auto project_node = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
        auto content_pane_node = ImGui::DockBuilderSplitNode(
            dockspace_id,
            ImGuiDir_Right,
            0.8f,
            nullptr,
            &dockspace_id
        );

        ImGui::DockBuilderDockWindow("ToolBar", tool_bar_node);
        ImGui::DockBuilderDockWindow("Project", project_node);
        ImGui::DockBuilderDockWindow("Script", dockspace_id);
        ImGui::DockBuilderDockWindow("Logs", logs_node);

        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::End();

    if (ImGui::Begin("ToolBar")) {

        if (m_game->is_paused()) {
            if (ImGui::Button("Resume")) {
                m_game->resume();
            }
        } else {
            if (ImGui::Button("Pause")) {
                m_game->pause();
            }
        }

        ImGui::SameLine();
        ImGui::Text("Resource Path: %s", m_game->get_resources()->get_base_path().c_str());

        ImGui::End();
    }

    if (ImGui::Begin("Project")) {

        ImGui::Text("Scripts (%zu)", m_scripts.size());

        if (ImGui::BeginListBox("##ScriptList")) {
            for (auto& script_info: m_scripts) {
                if (ImGui::Selectable(script_info.script->get_name().c_str(), &script_info == m_selected_script)) {
                    if (m_selected_script != &script_info) {
                        m_selected_script = &script_info;
                        if (m_selected_script->decompiled_source.empty()) {
                            m_selected_script->decompiled_source = decompile(m_game->get_runtime(), m_selected_script->script);
                        }
                    }
                }
            }
            ImGui::EndListBox();
        }

        ImGui::End();
    }

    if (m_selected_script) {
        if (ImGui::Begin("Script")) {
            ImGui::Text("%s (%s)", m_selected_script->script->get_name().c_str(), m_selected_script->script->get_path().c_str());
            ImGui::Separator();
            if (ImGui::BeginTabBar("##ScriptTabs")) {
                if (ImGui::BeginTabItem("Source")) {
                    ImGui::InputTextMultiline(
                        "##source",
                        const_cast<char*>(m_selected_script->formatted_source.c_str()), // yuck!!!
                        m_selected_script->formatted_source.size(),
                        ImVec2(-1.0f, -1.0f),
                        ImGuiInputTextFlags_ReadOnly
                    );
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("OpCodes")) {
                    ImGui::InputTextMultiline(
                        "##decompiled",
                        const_cast<char*>(m_selected_script->decompiled_source.c_str()), // yuck!!!
                        m_selected_script->decompiled_source.size(),
                        ImVec2(-1.0f, -1.0f),
                        ImGuiInputTextFlags_ReadOnly
                    );
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
//            ImGui::InputTextMultiline(
//                "##source",
//                const_cast<char*>(m_selected_script->formatted_source.c_str()), // yuck!!!
//                m_selected_script->formatted_source.size(),
//                ImVec2(-1.0f, -1.0f),
//                ImGuiInputTextFlags_ReadOnly
//            );

            ImGui::End();
        }
    }

    if (ImGui::Begin("Logs")) {
        auto& entries = m_memory_sink.get_entries();

        for (auto end = entries.end(); end != entries.begin();) {
            end--;

            auto& entry = *end;

            ImVec4 entry_color;

            switch (entry.level) {
                case LogLevel::Debug:
                    entry_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                    break;
                case LogLevel::Info:
                    entry_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case LogLevel::Warning:
                    entry_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                    break;
                case LogLevel::Error:
                    entry_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case LogLevel::Fatal:
                    entry_color = ImVec4(0.75f, 0.0f, 0.0f, 1.0f);
                    break;
            }
            ImGui::TextColored(entry_color,"%s", entry.message.c_str());
        }
        ImGui::End();
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    
    ImGui::Render();

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // NB: SDL_RenderXXX operations seem to work ok after calling `ImGui_ImplSDLRenderer2_RenderDrawData`
//    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
//    SDL_RenderDrawLine(m_renderer, 0, 0, 100, 100);

//    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
//    SDL_RenderClear(m_renderer);

    SDL_RenderPresent(m_renderer);
}

} // nomad