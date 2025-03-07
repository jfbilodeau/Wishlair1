//
// Created by jfbil on 2023-06-07.
//

#include "nomad/game/Game.hpp"

#include "nomad/compiler/Compiler.hpp"

#include "nomad/debug/DebugConsole.hpp"

#include "nomad/game/Alignment.hpp"
#include "nomad/game/Canvas.hpp"
#include "nomad/game/EntityVariableContext.hpp"
#include "nomad/game/Scene.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Texture.hpp"

#include "nomad/script/Runtime.hpp"

#include "nomad/system/FastHeap.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <utility>

#include <fstream>

namespace nomad {

const auto SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER;

Game::Game(NomadString resource_path, bool debug) :
    m_resource_path(std::move(resource_path)),
    m_debug(debug)
{
    init_sdl();
    init_sdl_ttf();
    init_resource_path();
    init_runtime();
    init_dynamic_variables();
    init_variable_context();
    init_commands();
    init_resource_manager();
    init_load_text();
    init_constants();

    set_language("en");

    compile_scripts();

    if (m_debug) {
        std::ofstream instruction_dump("instructions.txt");
        m_runtime->dump_instructions(instruction_dump);

        std::ofstream documentation_dump("documentation.md");
        m_runtime->dump_documentation(documentation_dump);
    }

    run_init_script();

    // Debug console must be initialized after the init script has been run
    init_debug_console();

    log::info("Game initialized");

    log::flush();
}

Game::~Game() {
    delete m_resource_manager;

    if (SDL_WasInit(SDL_INIT_FLAGS)) {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }

        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
        }

        SDL_Quit();
    }

    delete m_runtime;
}

Runtime* Game::get_runtime() const {
    return m_runtime;
}

ResourceManager* Game::get_resources() const {
    return m_resource_manager;
}

void Game::set_debug(bool debug) {
    m_debug = debug;
}

bool Game::is_debug() const {
    return m_debug;
}

const VariableMap* Game::get_scene_variables() const {
    return m_scene_variable_map;
}

const VariableMap* Game::get_entity_variables() const {
    return m_this_entity_variable_map;
}

const Point& Game::get_resolution() const {
    return m_resolution;
}

void Game::set_fps(NomadInteger fps) {
    m_fps = fps;
}

NomadInteger Game::get_fps() const {
    return m_fps;
}

const NomadString& Game::get_organization() const {
    return m_organization;
}

void Game::set_organization(const NomadString& organization) {
    m_organization = organization;
}

const NomadString& Game::get_name() const {
    return m_name;
}
void Game::set_name(const NomadString& name) {
    m_name = name;
}

NomadString Game::get_resource_path() const {
    return m_resource_path;
}

NomadString Game::get_state_path() const {
    if (m_organization.empty()) {
        log::error("game.organization not set");
        return "";
    }

    if (m_name.empty()) {
        log::error("game.name not set");
        return "";
    }

    const auto pref_path = SDL_GetPrefPath(m_organization.c_str(), m_name.c_str());
    
    if (pref_path == nullptr) {
        log::error("Failed to get state path: " + NomadString(SDL_GetError()));
        
        return "";
    }
    
    const NomadString state_path = pref_path;
    
    SDL_free(pref_path);
    
    return state_path;
}

NomadString Game::get_save_path() const {
    const auto path = get_state_path() + "saves";

    return path;
}

NomadString Game::get_settings_path() const {
    const auto path = get_state_path() + "settings";

    return path;
}

NomadString Game::make_resource_path(const NomadString& resource_name) const {
    const auto file_name = m_resource_path + resource_name;

    return file_name;
}

NomadString Game::make_state_path(const NomadString& state_name) const {
    const auto file_name = get_state_path() + state_name;

    create_path_to_file(file_name);

    return file_name;
}

NomadString Game::make_save_path(const NomadString& save_name) const {
    const auto file_name = get_save_path() + save_name;

    create_path_to_file(file_name);

    return file_name;
}

NomadString Game::make_settings_path(const NomadString& settings_name) const {
    const auto file_name = get_settings_path() + settings_name;

    create_path_to_file(file_name);

    return file_name;
}

bool Game::file_exists(const NomadString& file_name) const {
    return std::filesystem::exists(file_name);
}

void Game::delete_file(const NomadString& file_name) const {
    std::filesystem::remove(file_name);
}

bool Game::running() const {
    return m_running;
}

bool Game::is_paused() const {
    return m_paused;
}

void Game::pause() {
    m_paused = true;
}

void Game::resume() {
    m_paused = false;
}

void Game::run() {
    m_running = true;

    auto next_frame_time = SDL_GetTicks64() + (1000 / m_fps);

    while (m_running) {
        SDL_Event event{};

        while (SDL_PollEvent(&event) != 0) {
            if (m_debug_console != nullptr) {
//                m_debug_console->process_event(event);
            }

            switch (event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            if (event.window.windowID == SDL_GetWindowID(m_window)) {
                                m_running = false;
                            }
                            break;
                    }
                    break;

                case SDL_QUIT:
                    m_running = false;
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    process_input(event.key);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    process_input(event.button);
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    process_input(event.cbutton);
                    break;

                case SDL_MOUSEMOTION:
                    m_mouse_position.set(event.motion.x, event.motion.y);
                    break;

                default:
                    break;
            }
        }

        auto now = SDL_GetTicks64();

        if (next_frame_time <= now && m_running && !m_paused) {
            const auto update_start_time = SDL_GetTicks64();

            while (next_frame_time <= now) {
                next_frame_time = now + (1000 / m_fps);

                update();

                // Clear pressed/released state
                for (auto& button_state: m_mouse_button_state) {
                    button_state.pressed = false;
                    button_state.released = false;
                }
            }

            const auto update_end_time = SDL_GetTicks64();
            m_update_duration = update_end_time - update_start_time;

            const auto render_start_time = SDL_GetTicks64();
            render(m_canvas);

            SDL_RenderPresent(m_renderer);

            const auto render_end_time = SDL_GetTicks64();
            m_render_duration = render_end_time - render_start_time;

            m_mouse_last_position = m_mouse_position;
        }

        if (m_debug_console != nullptr) {
//            m_debug_console->render();
        }

        log::flush();
    }
}

void Game::quit() {
    m_running = false;
}

uint64_t Game::get_update_duration() const {
    return m_update_duration;
}

uint64_t Game::get_render_duration() const {
    return m_render_duration;
}

Canvas* Game::get_canvas() {
    return m_canvas;
}

NomadId Game::get_script_id(const NomadString& script_name) {
    return m_runtime->get_script_id(script_name);
}

void Game::execute_script_in_current_context(NomadId script_id) {
    m_runtime->execute_script(script_id);
}

void Game::execute_script_in_current_context(NomadId script_id, ScriptValue& return_value) {
    m_runtime->execute_script(script_id, return_value);
}

void Game::execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity) {
    push_execution_context(scene, entity);

    m_runtime->execute_script(script_id);

    pop_execution_context();
}

void Game::execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, Entity* other) {
    push_execution_context(scene, entity, other);

    m_runtime->execute_script(script_id);

    pop_execution_context();
}

void Game::execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, const std::vector<Entity*>& other_entities) {
    push_execution_context(scene, entity, other_entities);

    m_runtime->execute_script(script_id);

    pop_execution_context();
}

void Game::execute_script_in_new_context(NomadId script_id, Scene* scene, Entity* entity, ScriptValue& return_value) {
    push_execution_context(scene, entity);

    m_runtime->execute_script(script_id, return_value);

    pop_execution_context();
}

void Game::execute_script_in_context(NomadId script_id, GameExecutionContext* context) {
    const auto previous_context = m_current_context;

    m_current_context = context;

    m_runtime->execute_script(script_id);

    m_current_context = previous_context;
}

void Game::execute_script_in_context(NomadId script_id, GameExecutionContext* context, ScriptValue& return_value) {
    const auto previous_context = m_current_context;

    m_current_context = context;

    m_runtime->execute_script(script_id, return_value);

    m_current_context = previous_context;
}

void Game::execute_script_by_name(const NomadString& script_name, Scene* scene, Entity* entity, ScriptValue& return_value) {
    const auto script_id = get_script_id(script_name);

    if (script_id == NOMAD_INVALID_ID) {
        raise_error("Script '" + script_name + "' not found");
    }

    execute_script_in_new_context(script_id, scene, entity, return_value);
}

bool Game::execute_predicate(NomadId script_id) {
    ScriptValue return_value;

    m_runtime->execute_script(script_id, return_value);

    return return_value.get_boolean_value();
}

Scene* Game::create_scene() {
    const auto scene = new Scene(this);

    m_added_scenes.push_back(scene);

    return scene;
}

void Game::remove_scene(Scene* scene) {
    m_removed_scenes.push_back(scene);
}

void Game::raise_error(const NomadString& message) {
    log::error(message);
    throw GameException(message);
}

void Game::init_sdl() {
    log::info("Initializing SDL2");

    if (SDL_Init(SDL_INIT_FLAGS)) {
        raise_error("Failed to initialize SDL2: " + NomadString(SDL_GetError()));
    }

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_window_size.x(),
        m_window_size.y(),
        SDL_WINDOW_RESIZABLE
    );

    if (m_window == nullptr) {
        raise_error("Failed to create SDL window. Reason: " + NomadString(SDL_GetError()));
    }

    m_renderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (m_renderer == nullptr) {
        raise_error("Failed to create SDL renderer. Reason: " + NomadString(SDL_GetError()));
    }

    if (SDL_RenderSetLogicalSize(m_renderer, m_resolution.x(), m_resolution.y()) != 0) {
        raise_error("Failed to set logical size: " + NomadString(SDL_GetError()));
    }

    m_canvas = new Canvas(this, m_renderer);
}

void Game::init_sdl_ttf() {
    const auto result = TTF_Init();

    if (result != 0) {
        NomadString ttf_message = TTF_GetError();

        NomadString error_message = "Failed to initialize SDL_ttf. Reason: " + ttf_message;

        throw GameException(error_message);
    }
}

void Game::init_resource_path() {
    // Get rid of the trailing backslash
    if (m_resource_path[m_resource_path.size() - 1] == '\\') {
        m_resource_path = m_resource_path.substr(0, m_resource_path.size() - 1);
    }

    // Ensure non-empty resource path ends with a slash
    auto resource_path_end = m_resource_path[m_resource_path.size() - 1];
    if (resource_path_end != '/') {
        m_resource_path += '/';
    }

    log::info("Resource path: " + m_resource_path);
}

void Game::init_runtime() {
    log::info("Initializing script runtime");

    m_runtime = new Runtime();

    m_runtime->set_debug(m_debug);
}

void Game::init_commands() {
    log::info("Initializing commands");

    init_game_commands();
    init_input_commands();
    init_other_entity_commands();
    init_scene_commands();
    init_this_entity_commands();
    init_window_commands();
}

void Game::init_dynamic_variables() {
    log::info("Initializing dynamic variables");

    init_game_dynamic_variables();
    init_input_dynamic_variables();
    init_other_dynamic_variables();
    init_scene_dynamic_variables();
    init_this_dynamic_variables();
    init_window_dynamic_variables();
}

void Game::init_variable_context() {
    auto scene_variable_context = std::make_unique<SimpleVariableContext>();
    auto this_entity_variable_context = std::make_unique<ThisEntityVariableContext>(this);
    auto other_entity_variable_context = std::make_unique<OtherEntityVariableContext>(this_entity_variable_context.get());

    m_scene_variable_map = scene_variable_context->get_variable_map();
    m_this_entity_variable_map = this_entity_variable_context->get_this_variable_map();
    m_other_entity_variable_map = this_entity_variable_context->get_other_variable_map();

    auto scene_context_id = m_runtime->register_variable_context("scene", "scene.", std::move(scene_variable_context));
    auto this_entity_context_id = m_runtime->register_variable_context(THIS_ENTITY_VARIABLE_CONTEXT, THIS_ENTITY_VARIABLE_PREFIX, std::move(this_entity_variable_context));
    auto other_entity_context_id = m_runtime->register_variable_context(OTHER_ENTITY_VARIABLE_CONTEXT, OTHER_ENTITY_VARIABLE_PREFIX, std::move(other_entity_variable_context));
}

void Game::init_resource_manager() {
    log::info("Initializing resource manager");

    m_resource_manager = new ResourceManager(this, m_resource_path);
}

void Game::init_constants() {
    log::info("Initializing constants");

    m_runtime->register_constant("alignment.topLeft", ScriptValue(static_cast<NomadInteger>(Alignment::TopLeft)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.topMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::TopMiddle)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.topRight", ScriptValue(static_cast<NomadInteger>(Alignment::TopRight)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.centerLeft", ScriptValue(static_cast<NomadInteger>(Alignment::CenterLeft)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.centerMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::CenterMiddle)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.centerRight", ScriptValue(static_cast<NomadInteger>(Alignment::CenterRight)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.bottomLeft", ScriptValue(static_cast<NomadInteger>(Alignment::BottomLeft)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.bottomMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::BottomMiddle)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.bottomRight", ScriptValue(static_cast<NomadInteger>(Alignment::BottomRight)), m_runtime->get_integer_type());

    m_runtime->register_constant("alignment.left", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Left)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.middle", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Middle)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.right", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Right)), m_runtime->get_integer_type());

    m_runtime->register_constant("alignment.top", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Top)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.center", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Center)), m_runtime->get_integer_type());
    m_runtime->register_constant("alignment.bottom", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Bottom)), m_runtime->get_integer_type());

    m_runtime->register_constant("body.static", ScriptValue(static_cast<NomadInteger>(BodyType::Static)), m_runtime->get_integer_type());
    m_runtime->register_constant("body.dynamic", ScriptValue(static_cast<NomadInteger>(BodyType::Dynamic)), m_runtime->get_integer_type());
    m_runtime->register_constant("body.kinematic", ScriptValue(static_cast<NomadInteger>(BodyType::Kinematic)), m_runtime->get_integer_type());

    m_runtime->register_constant("cardinal.north", ScriptValue(static_cast<NomadInteger>(Cardinal::North)), m_runtime->get_integer_type());
    m_runtime->register_constant("cardinal.east", ScriptValue(static_cast<NomadInteger>(Cardinal::East)), m_runtime->get_integer_type());
    m_runtime->register_constant("cardinal.south", ScriptValue(static_cast<NomadInteger>(Cardinal::South)), m_runtime->get_integer_type());
    m_runtime->register_constant("cardinal.west", ScriptValue(static_cast<NomadInteger>(Cardinal::West)), m_runtime->get_integer_type());
}

void Game::init_load_text() {
    log::info("Loading text...");

    m_resource_manager->get_text()->load_text_from_csv(m_resource_path + "text/text.csv");
}

void Game::init_debug_console() {
    // Move to lazy-init
    // log::info("Initializing debug console");
    //
    // m_debug_console = new DebugConsole(this);
    // m_debug_console->activate();
}

//NomadString Game::make_path(const NomadString& path) const {
void Game::create_path_to_file(const NomadString& file_name) const {
    const auto path = std::filesystem::path(file_name).parent_path();

    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

void Game::load_text_into_const(const NomadString& language_code) {
    std::unordered_map<NomadString, NomadString> text_map;

    m_resource_manager->get_text()->get_all_text(language_code, text_map);

    auto constant_type = m_runtime->get_string_type();
    auto constant_value = ScriptValue();

    for (const auto& [key, value]: text_map) {
        auto constant_name = "t." + key;

        constant_value.set_string_value(value);

        m_runtime->register_constant(constant_name, constant_value, constant_type);
    }
}

void Game::compile_scripts() {
    log::info("Compiling scripts...");

    const auto start_ticks = SDL_GetTicks64();

    auto compiler = m_runtime->create_compiler();

    try {
        compiler->load_scripts_from_path(m_resource_path + "scripts");
        compiler->load_scripts_from_path(m_resource_path + "mods");

        compiler->compile_scripts();
    } catch (NomadException& e) {
        raise_error(NomadString("Failed to compile scripts: ") + e.what());
    } catch (std::exception& e) {
        raise_error(NomadString("Unexpected exception while compiling scripts: ") + e.what());
    }

    const auto end_ticks = SDL_GetTicks64();
    const auto elapsed_ticks = end_ticks - start_ticks;
    const auto script_count = m_runtime->get_script_count();

    const auto message =
        NomadString("Compiled ") +
        std::to_string(script_count) +
        " scripts in " +
        std::to_string(elapsed_ticks) +
        "ms";

    log::info(message);
}

void Game::push_execution_context(Scene* scene, Entity* entity) {
    m_context_index++;

    if (m_context_index >= m_context_stack.size()) {
        raise_error("Execution context stack overflow");
    }

    m_current_context = &m_context_stack[m_context_index];
    m_current_context->reset(scene, entity);
}

void Game::push_execution_context(Scene* scene, Entity* entity, Entity* other) {
    m_context_index++;

    if (m_context_index >= m_context_stack.size()) {
        raise_error("Execution context stack overflow");
    }

    m_current_context = &m_context_stack[m_context_index];
    m_current_context->reset(scene, entity);
    m_current_context->add_other_entity(other);
}

void Game::push_execution_context(Scene* scene, Entity* entity, const std::vector<Entity*>& other_entities) {
    m_context_index++;

    if (m_context_index >= m_context_stack.size()) {
        raise_error("Execution context stack overflow");
    }

    m_current_context = &m_context_stack[m_context_index];
    m_current_context->reset(scene, entity);
    m_current_context->set_other_entities(other_entities);
}

void Game::pop_execution_context() {
    m_context_index--;

    if (m_context_index < 0) {
        raise_error("Execution context stack underflow");
    }

    m_current_context = &m_context_stack[m_context_index];
}

GameExecutionContext* Game::get_current_context() {
//    return &m_context_stack[m_context_index];
    return m_current_context;
}

void Game::set_language(const NomadString& language_code) {
    m_language = language_code;

    load_text_into_const(language_code);
}

const NomadString& Game::get_language() const {
    return m_language;
}

NomadString Game::get_text(const NomadString& key) const {
    if (m_debug) {
        if (m_resource_manager->get_text()->get_text(m_language, key).empty()) {
            log::warning("Missing text for key: " + key);

            return "<<" + key + ">>";
        }
    }

    auto text = m_resource_manager->get_text()->get_text(m_language, key);

    return text;
}

void Game::run_init_script() {
    log::info("Executing `init` script");

    NomadId init_script_id = m_runtime->get_script_id("init");

    if (init_script_id == NOMAD_INVALID_ID) {
        const char* message = "Fatal: No `init` script found";

        log::fatal(message);

        exit(EXIT_FAILURE);
    }

    try {
        execute_script_in_current_context(init_script_id);
    } catch (std::exception& e) {
        raise_error(NomadString("Failed to execute 'init' script: ") + e.what());
    }
}

void Game::render(Canvas* canvas) {
    canvas->clear(Colors::White);

    for (const auto scene: m_scenes) {
        scene->render(canvas);
    }
}

void Game::update() {
    for (const auto scene: m_scenes) {
        scene->update(this);
    }

    for (auto added_scene: m_added_scenes) {
        m_scenes.push_back(added_scene);
    }
    m_added_scenes.clear();

    for (auto scene: m_removed_scenes) {
        const auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
        if (it != m_scenes.end()) {
            m_scenes.erase(it);
        }
    }
    m_removed_scenes.clear();

    // Sort scenes by layer
    std::sort(m_scenes.begin(), m_scenes.end(), [](Scene* a, Scene* b) {
        return a->get_z() < b->get_z();
    });

    fast_temp_heap_reset();
}

bool Game::process_system_input(SDL_KeyboardEvent& event) {
    switch (event.type) {
    case SDL_KEYUP:
        switch (event.keysym.sym) {
            // Fullscreen toggle
        case SDLK_F11:
            if ((SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN) != 0) {
                SDL_SetWindowFullscreen(m_window, 0);
            } else {
                SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            return true;

            // Debug console toggle
        case SDLK_F12:
            if (m_debug) {  // Only active in debug mode
                if (m_debug_console == nullptr) {
                    log::info("Initializing debug console");

//                    m_debug_console = new DebugConsole(this);
                }

//                if (m_debug_console->is_active()) {
//                    m_debug_console->deactivate();
//                } else {
//                    m_debug_console->activate();
//                }
            }
            return true;;
        }
    }

    return false;
}

void Game::process_input(SDL_KeyboardEvent& event) {
    // Process system input.
    if (process_system_input(event)) {
        return;
    }

    InputEvent input_event;

    map_sdl_keyboard_event(event, input_event);

    dispatch_input_event(input_event);
}

void Game::process_input(SDL_MouseButtonEvent& event) {
    if (event.button <= m_mouse_button_state.size()) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                m_mouse_button_state[event.button].pressed = true;
                m_mouse_button_state[event.button].held = true;
                break;

            case SDL_MOUSEBUTTONUP:
                m_mouse_button_state[event.button].released = true;
                m_mouse_button_state[event.button].held = false;
                break;
        }
    }

    InputEvent input_event;

    map_sdl_mouse_event(event, input_event);

    dispatch_input_event(input_event);
}

void Game::process_input(SDL_ControllerButtonEvent& event) const {
    InputEvent input_event;

    map_sdl_controller_button_event(event, input_event);

    dispatch_input_event(input_event);
}

void Game::dispatch_input_event(const InputEvent& event) const {
    for (const auto& scene: m_scenes) {
        scene->process_input_event(event);
    }
}

Scene* Game::get_current_scene() {
    return get_current_context()->get_scene();
}

} // nomad