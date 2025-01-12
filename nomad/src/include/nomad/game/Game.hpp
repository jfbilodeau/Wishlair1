//
// Created by jfbil on 2023-06-07.
//

#ifndef NOMAD_GAME_HPP
#define NOMAD_GAME_HPP

#include "nomad/nomad.hpp"
#include "nomad/log/Logger.hpp"

#include "nomad/geometry/Point.hpp"
#include "nomad/geometry/PointF.hpp"

#include "nomad/game/GameExecutionContext.hpp"
#include "nomad/game/InputManager.hpp"

#include <SDL.h>

#include <array>

#include "nomad/script/ScriptValue.hpp"
#include "nomad/script/Variable.hpp"

namespace nomad {

// Forward declarations
class DebugConsole;
class Canvas;
class EntityVariableContext;
class InputManager;
class Runtime;
class Resource;
class ResourceManager;
class Scene;
class Script;
class SimpleVariableContext;

class GameException : public NomadException {
public:
    explicit GameException(const NomadString& message) : NomadException(message) {}
};

class Game {
public:
    Game(NomadString resource_path, bool debug);
    Game(const Game&) = delete;
    ~Game();

    [[nodiscard]] Runtime* get_runtime() const;
    [[nodiscard]] Canvas* get_canvas();
    [[nodiscard]] ResourceManager* get_resources() const;

    void set_debug(bool debug);
    [[nodiscard]] bool is_debug() const;

    [[nodiscard]] const VariableMap* get_scene_variables() const;
    [[nodiscard]] const VariableMap* get_entity_variables() const;

    [[nodiscard]] const Point& get_resolution() const;
    void set_fps(NomadInteger fps);
    [[nodiscard]] NomadInteger get_fps() const;

    [[nodiscard]] const NomadString& get_organization() const;
    void set_organization(const NomadString& organization);

    [[nodiscard]] const NomadString& get_name() const;
    void set_name(const NomadString& name);

    [[nodiscard]] NomadString get_resource_path() const;
    [[nodiscard]] NomadString get_state_path() const;
    [[nodiscard]] NomadString get_save_path() const;
    [[nodiscard]] NomadString get_settings_path() const;

    [[nodiscard]] NomadString make_resource_path(const NomadString& resource_name) const;
    [[nodiscard]] NomadString make_state_path(const NomadString& state_name) const;
    [[nodiscard]] NomadString make_save_path(const NomadString& save_name) const;
    [[nodiscard]] NomadString make_settings_path(const NomadString& settings_name) const;

    [[nodiscard]] bool file_exists(const NomadString& file_name) const;
    void delete_file(const NomadString& file_name) const;

    void run();
    void quit();
    [[nodiscard]] uint64_t get_update_duration() const;
    [[nodiscard]] uint64_t get_render_duration() const;

    [[nodiscard]] NomadId get_script_id(const NomadString& script_name);
    void execute_script_in_current_context(NomadId script_id);
    void execute_script_in_current_context(NomadId script_id, ScriptValue& return_value);
    void execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity);
    void execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, Entity* other);
    void execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, const std::vector<Entity*>& others);
    void execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, ScriptValue& return_value);
    void execute_script_in_context(NomadId script_id, GameExecutionContext* context);
    void execute_script_in_context(NomadId script_id, GameExecutionContext* context, ScriptValue& return_value);
    void execute_script_by_name(const NomadString& script_name, Scene* scene, Entity* entity, ScriptValue& return_value);

    [[nodiscard]] bool execute_predicate(NomadId script_id);

    Scene* create_scene();
    void remove_scene(Scene* scene);

    [[nodiscard]] bool running() const;
    [[nodiscard]] bool is_paused() const;

    void pause();
    void resume();

    GameExecutionContext* get_current_context();

    void set_language(const NomadString& language_code);
    [[nodiscard]] const NomadString& get_language() const;
    [[nodiscard]] NomadString get_text(const NomadString& key) const;

    [[noreturn]] void raise_error(const NomadString& message);

private:
    void init_sdl();
    void init_sdl_ttf();
    void init_resource_path();
    void init_runtime();

    void init_commands();
    void init_entity_commands();
    void init_game_commands();
    void init_input_commands();
    void init_scene_commands();
    void init_window_commands();
    void init_variable_context();
    void init_resource_manager();
    void init_constants();
    void init_load_text();
    void init_debug_console();

    // Init dynamic variables
    void init_dynamic_variables();
    void init_game_dynamic_variables();
    void init_input_dynamic_variables();
    void init_other_dynamic_variables();
    void init_scene_dynamic_variables();
    void init_this_dynamic_variables();
    void init_window_dynamic_variables();

    void create_path_to_file(const NomadString& file_name) const;
    void load_text_into_const(const NomadString& language_code);

    void compile_scripts();

    void push_execution_context(Scene* scene, Entity* entity);
    void push_execution_context(Scene* scene, Entity* entity, Entity* other);
    void push_execution_context(Scene* scene, Entity* entity, const std::vector<Entity*>& others);
    void pop_execution_context();
    void run_init_script();

    void render(Canvas* canvas);
    void update();

    void process_input(SDL_KeyboardEvent& event);
    bool process_system_input(SDL_KeyboardEvent& event);
    void process_input(SDL_MouseButtonEvent& event);
    void process_input(SDL_ControllerButtonEvent& event) const;
    void dispatch_input_event(const InputEvent& event) const;

    // The current scene being processed
    [[nodiscard]] Scene* get_current_scene();

    NomadString m_title = "Nomad";
    Point m_resolution = {800, 600};
    Point m_window_size = m_resolution;
    int m_fps = 30;

//    std::unique_ptr<SimpleVariableContext> m_scene_variable_context;
//    std::unique_ptr<EntityVariableContext> m_entity_variable_context;
    const VariableMap* m_scene_variable_map = nullptr;
    const VariableMap* m_entity_variable_map = nullptr;

    PointF m_mouse_position;
    struct MouseButtonState {
        bool pressed = false;
        bool held = false;
        bool released = false;
    };
    std::array<MouseButtonState, 3> m_mouse_button_state = {
        MouseButtonState{},
        MouseButtonState{},
        MouseButtonState{},
    };

    PointF m_mouse_last_position;
    ResourceManager* m_resource_manager = nullptr;

    std::vector<Scene*> m_added_scenes;
    std::vector<Scene*> m_removed_scenes;
    std::vector<Scene*> m_scenes;

    NomadString m_resource_path;
    NomadString m_organization;
    NomadString m_name;

    bool m_debug = false;
    bool m_running = false;
    bool m_paused = false;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    Canvas* m_canvas = nullptr;

    // Performance
    uint64_t m_render_duration = 0;
    uint64_t m_update_duration = 0;

    InputManager* m_input_manager = nullptr;

    DebugConsole* m_debug_console = nullptr;

    Runtime* m_runtime = nullptr;
    std::array<GameExecutionContext, 10> m_context_stack = {
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
        GameExecutionContext{},
    };
    GameExecutionContext* m_current_context = nullptr;
    int m_context_index = 0;

    NomadString m_language = "en";

//    std::vector<Resource*> m_resources;
};

} // nomad

#endif //NOMAD_GAME_HPP
