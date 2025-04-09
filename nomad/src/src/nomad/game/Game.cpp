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

#include "nomad/system/TempHeap.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <utility>

#include <fstream>

namespace nomad {

const auto SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER;

Game::Game(NomadString resource_path, bool debug) :
    m_resourcePath(std::move(resource_path)),
    m_debug(debug)
{
    initSdl();
    initSdlTtf();
    initResourcePath();
    initRuntime();
    initDynamicVariables();
    initVariableContext();
    initCommands();
    initResourceManager();
    initLoadText();
    initConstants();

    setLanguage("en");

    compileScripts();

    if (m_debug) {
        std::ofstream instruction_dump("instructions.txt");
        m_runtime->dumpInstructions(instruction_dump);

        std::ofstream documentation_dump("documentation.md");
        m_runtime->dumpDocumentation(documentation_dump);
    }

    runInitScript();

    // Debug console must be initialized after the init script has been run
    initDebugConsole();

    log::info("Game initialized");

    log::flush();
}

Game::~Game() {
    delete m_resourceManager;

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

Runtime* Game::getRuntime() const {
    return m_runtime;
}

ResourceManager* Game::getResources() const {
    return m_resourceManager;
}

void Game::setDebug(bool debug) {
    m_debug = debug;
}

bool Game::isDebug() const {
    return m_debug;
}

const VariableMap* Game::getSceneVariables() const {
    return m_sceneVariableMap;
}

const VariableMap* Game::getEntityVariables() const {
    return m_thisEntityVariableMap;
}

const Point& Game::getResolution() const {
    return m_resolution;
}

void Game::setFps(NomadInteger fps) {
    m_fps = fps;
}

NomadInteger Game::getFps() const {
    return m_fps;
}

const NomadString& Game::getOrganization() const {
    return m_organization;
}

void Game::setOrganization(const NomadString& organization) {
    m_organization = organization;
}

const NomadString& Game::getName() const {
    return m_name;
}
void Game::setName(const NomadString& name) {
    m_name = name;
}

NomadString Game::getResourcePath() const {
    return m_resourcePath;
}

NomadString Game::getStatePath() const {
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

NomadString Game::getSavePath() const {
    const auto path = getStatePath() + "saves";

    return path;
}

NomadString Game::getSettingsPath() const {
    const auto path = getStatePath() + "settings";

    return path;
}

NomadString Game::makeResourcePath(const NomadString& resourceName) const {
    const auto file_name = m_resourcePath + resourceName;

    return file_name;
}

NomadString Game::makeStatePath(const NomadString& stateName) const {
    const auto file_name = getStatePath() + stateName;

    createPathToFile(file_name);

    return file_name;
}

NomadString Game::makeSavePath(const NomadString& saveName) const {
    const auto file_name = getSavePath() + saveName;

    createPathToFile(file_name);

    return file_name;
}

NomadString Game::makeSettingsPath(const NomadString& settingsName) const {
    const auto file_name = getSettingsPath() + settingsName;

    createPathToFile(file_name);

    return file_name;
}

bool Game::fileExists(const NomadString& fileName) const {
    return std::filesystem::exists(fileName);
}

void Game::deleteFile(const NomadString& fileName) const {
    std::filesystem::remove(fileName);
}

bool Game::running() const {
    return m_running;
}

bool Game::isPaused() const {
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
            if (m_debugConsole != nullptr) {
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
                    processInput(event.key);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    processInput(event.button);
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    processInput(event.cbutton);
                    break;

                case SDL_MOUSEMOTION:
                    m_mousePosition.set(event.motion.x, event.motion.y);
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
                for (auto& button_state: m_mouseButtonState) {
                    button_state.pressed = false;
                    button_state.released = false;
                }
            }

            const auto update_end_time = SDL_GetTicks64();
            m_updateDuration = update_end_time - update_start_time;

            const auto render_start_time = SDL_GetTicks64();
            render(m_canvas);

            SDL_RenderPresent(m_renderer);

            const auto render_end_time = SDL_GetTicks64();
            m_renderDuration = render_end_time - render_start_time;

            m_mouseLastPosition = m_mousePosition;
        }

        if (m_debugConsole != nullptr) {
//            m_debug_console->render();
        }

        log::flush();
    }
}

void Game::quit() {
    m_running = false;
}

uint64_t Game::getUpdateDuration() const {
    return m_updateDuration;
}

uint64_t Game::getRenderDuration() const {
    return m_renderDuration;
}

Canvas* Game::getCanvas() {
    return m_canvas;
}

NomadId Game::getScriptId(const NomadString& scriptName) {
    return m_runtime->getScriptId(scriptName);
}

void Game::executeScriptInCurrentContext(NomadId scriptId) {
    m_runtime->executeScript(scriptId);
}

void Game::executeScriptInCurrentContext(NomadId scriptId, ScriptValue& returnValue) {
    m_runtime->executeScript(scriptId, returnValue);
}

void Game::executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity) {
    pushExecutionContext(scene, entity);

    m_runtime->executeScript(scriptId);

    popExecutionContext();
}

void Game::executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, Entity* other) {
    pushExecutionContext(scene, entity, other);

    m_runtime->executeScript(scriptId);

    popExecutionContext();
}

void Game::executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, const std::vector<Entity*>& other_entities) {
    pushExecutionContext(scene, entity, other_entities);

    m_runtime->executeScript(scriptId);

    popExecutionContext();
}

void Game::executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, ScriptValue& returnValue) {
    pushExecutionContext(scene, entity);

    m_runtime->executeScript(scriptId, returnValue);

    popExecutionContext();
}

void Game::executeScriptInContext(NomadId scriptId, GameExecutionContext* context) {
    const auto previous_context = m_currentContext;

    m_currentContext = context;

    m_runtime->executeScript(scriptId);

    m_currentContext = previous_context;
}

void Game::executeScriptInContext(NomadId scriptId, GameExecutionContext* context, ScriptValue& returnValue) {
    const auto previous_context = m_currentContext;

    m_currentContext = context;

    m_runtime->executeScript(scriptId, returnValue);

    m_currentContext = previous_context;
}

void Game::executeScriptByName(const NomadString& scriptName, Scene* scene, Entity* entity, ScriptValue& returnValue) {
    const auto script_id = getScriptId(scriptName);

    if (script_id == NOMAD_INVALID_ID) {
        raiseError("Script '" + scriptName + "' not found");
    }

    executeScriptInNewContext(script_id, scene, entity, returnValue);
}

bool Game::executePredicate(NomadId script_id) {
    ScriptValue return_value;

    m_runtime->executeScript(script_id, return_value);

    return return_value.getBooleanValue();
}

Scene* Game::createScene() {
    const auto scene = new Scene(this);

    m_addedScenes.push_back(scene);

    return scene;
}

void Game::removeScene(Scene* scene) {
    m_removedScenes.push_back(scene);
}

void Game::raiseError(const NomadString& message) {
    log::error(message);
    throw GameException(message);
}

void Game::initSdl() {
    log::info("Initializing SDL2");

    if (SDL_Init(SDL_INIT_FLAGS)) {
        raiseError("Failed to initialize SDL2: " + NomadString(SDL_GetError()));
    }

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_windowSize.getX(),
        m_windowSize.getY(),
        SDL_WINDOW_RESIZABLE
    );

    if (m_window == nullptr) {
        raiseError("Failed to create SDL window. Reason: " + NomadString(SDL_GetError()));
    }

    m_renderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (m_renderer == nullptr) {
        raiseError("Failed to create SDL renderer. Reason: " + NomadString(SDL_GetError()));
    }

    if (SDL_RenderSetLogicalSize(m_renderer, m_resolution.getX(), m_resolution.getY()) != 0) {
        raiseError("Failed to set logical size: " + NomadString(SDL_GetError()));
    }

    m_canvas = new Canvas(this, m_renderer);
}

void Game::initSdlTtf() {
    const auto result = TTF_Init();

    if (result != 0) {
        NomadString ttf_message = TTF_GetError();

        NomadString error_message = "Failed to initialize SDL_ttf. Reason: " + ttf_message;

        throw GameException(error_message);
    }
}

void Game::initResourcePath() {
    // Get rid of the trailing backslash
    if (m_resourcePath[m_resourcePath.size() - 1] == '\\') {
        m_resourcePath = m_resourcePath.substr(0, m_resourcePath.size() - 1);
    }

    // Ensure non-empty resource path ends with a slash
    auto resource_path_end = m_resourcePath[m_resourcePath.size() - 1];
    if (resource_path_end != '/') {
        m_resourcePath += '/';
    }

    log::info("Resource path: " + m_resourcePath);
}

void Game::initRuntime() {
    log::info("Initializing script runtime");

    m_runtime = new Runtime();

    m_runtime->setDebug(m_debug);
}

void Game::initCommands() {
    log::info("Initializing commands");

    initGameCommands();
    initInputCommands();
    initOtherEntityCommands();
    initSceneCommands();
    initThisEntityCommands();
    initWindowCommands();
}

void Game::initDynamicVariables() {
    log::info("Initializing dynamic variables");

    initGameDynamicVariables();
    initInputDynamicVariables();
    initOtherDynamicVariables();
    initSceneDynamicVariables();
    initThisDynamicVariables();
    initWindowDynamicVariables();
}

void Game::initVariableContext() {
    auto scene_variable_context = std::make_unique<SimpleVariableContext>();
    auto this_entity_variable_context = std::make_unique<ThisEntityVariableContext>(this);
    auto other_entity_variable_context = std::make_unique<OtherEntityVariableContext>(this_entity_variable_context.get());

    m_sceneVariableMap = scene_variable_context->get_variable_map();
    m_thisEntityVariableMap = this_entity_variable_context->getThisVariableMap();
    m_otherEntityVariableMap = this_entity_variable_context->getOtherVariableMap();

    auto scene_context_id = m_runtime->registerVariableContext("scene", "scene.", std::move(scene_variable_context));
    auto this_entity_context_id = m_runtime->registerVariableContext(THIS_ENTITY_VARIABLE_CONTEXT, THIS_ENTITY_VARIABLE_PREFIX, std::move(this_entity_variable_context));
    auto other_entity_context_id = m_runtime->registerVariableContext(OTHER_ENTITY_VARIABLE_CONTEXT, OTHER_ENTITY_VARIABLE_PREFIX, std::move(other_entity_variable_context));
}

void Game::initResourceManager() {
    log::info("Initializing resource manager");

    m_resourceManager = new ResourceManager(this, m_resourcePath);
}

void Game::initConstants() {
    log::info("Initializing constants");

    m_runtime->registerConstant("alignment.topLeft", ScriptValue(static_cast<NomadInteger>(Alignment::TopLeft)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.topMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::TopMiddle)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.topRight", ScriptValue(static_cast<NomadInteger>(Alignment::TopRight)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.centerLeft", ScriptValue(static_cast<NomadInteger>(Alignment::CenterLeft)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.centerMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::CenterMiddle)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.centerRight", ScriptValue(static_cast<NomadInteger>(Alignment::CenterRight)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.bottomLeft", ScriptValue(static_cast<NomadInteger>(Alignment::BottomLeft)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.bottomMiddle", ScriptValue(static_cast<NomadInteger>(Alignment::BottomMiddle)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.bottomRight", ScriptValue(static_cast<NomadInteger>(Alignment::BottomRight)), m_runtime->getIntegerType());

    m_runtime->registerConstant("alignment.left", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Left)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.middle", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Middle)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.right", ScriptValue(static_cast<NomadInteger>(HorizontalAlignment::Right)), m_runtime->getIntegerType());

    m_runtime->registerConstant("alignment.top", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Top)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.center", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Center)), m_runtime->getIntegerType());
    m_runtime->registerConstant("alignment.bottom", ScriptValue(static_cast<NomadInteger>(VerticalAlignment::Bottom)), m_runtime->getIntegerType());

    m_runtime->registerConstant("body.static", ScriptValue(static_cast<NomadInteger>(BodyType::Static)), m_runtime->getIntegerType());
    m_runtime->registerConstant("body.dynamic", ScriptValue(static_cast<NomadInteger>(BodyType::Dynamic)), m_runtime->getIntegerType());
    m_runtime->registerConstant("body.kinematic", ScriptValue(static_cast<NomadInteger>(BodyType::Kinematic)), m_runtime->getIntegerType());

    m_runtime->registerConstant("cardinal.north", ScriptValue(static_cast<NomadInteger>(Cardinal::North)), m_runtime->getIntegerType());
    m_runtime->registerConstant("cardinal.east", ScriptValue(static_cast<NomadInteger>(Cardinal::East)), m_runtime->getIntegerType());
    m_runtime->registerConstant("cardinal.south", ScriptValue(static_cast<NomadInteger>(Cardinal::South)), m_runtime->getIntegerType());
    m_runtime->registerConstant("cardinal.west", ScriptValue(static_cast<NomadInteger>(Cardinal::West)), m_runtime->getIntegerType());
}

void Game::initLoadText() {
    log::info("Loading text...");

    m_resourceManager->getText()->loadTextFromCsv(m_resourcePath + "text/text.csv");
}

void Game::initDebugConsole() {
    // Move to lazy-init
    // log::info("Initializing debug console");
    //
    // m_debug_console = new DebugConsole(this);
    // m_debug_console->activate();
}

//NomadString Game::make_path(const NomadString& path) const {
void Game::createPathToFile(const NomadString& fileName) const {
    const auto path = std::filesystem::path(fileName).parent_path();

    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

void Game::loadTextIntoConst(const NomadString& languageCode) {
    std::unordered_map<NomadString, NomadString> text_map;

    m_resourceManager->getText()->getAllText(languageCode, text_map);

    auto constant_type = m_runtime->getStringType();
    auto constant_value = ScriptValue();

    for (const auto& [key, value]: text_map) {
        auto constant_name = "t." + key;

        constant_value.setStringValue(value);

        m_runtime->registerConstant(constant_name, constant_value, constant_type);
    }
}

void Game::compileScripts() {
    log::info("Compiling scripts...");

    const auto start_ticks = SDL_GetTicks64();

    auto compiler = m_runtime->createCompiler();

    try {
        compiler->loadScriptsFromPath(m_resourcePath + "scripts");
        compiler->loadScriptsFromPath(m_resourcePath + "mods");

        compiler->compileScripts();
    } catch (NomadException& e) {
        raiseError(NomadString("Failed to compile scripts: ") + e.what());
    } catch (std::exception& e) {
        raiseError(NomadString("Unexpected exception while compiling scripts: ") + e.what());
    }

    const auto end_ticks = SDL_GetTicks64();
    const auto elapsed_ticks = end_ticks - start_ticks;
    const auto script_count = m_runtime->getScriptCount();

    const auto message =
        NomadString("Compiled ") +
        std::to_string(script_count) +
        " scripts in " +
        std::to_string(elapsed_ticks) +
        "ms";

    log::info(message);
}

void Game::pushExecutionContext(Scene* scene, Entity* entity) {
    m_contextIndex++;

    if (m_contextIndex >= m_contextStack.size()) {
        raiseError("Execution context stack overflow");
    }

    m_currentContext = &m_contextStack[m_contextIndex];
    m_currentContext->reset(scene, entity);
}

void Game::pushExecutionContext(Scene* scene, Entity* entity, Entity* other) {
    m_contextIndex++;

    if (m_contextIndex >= m_contextStack.size()) {
        raiseError("Execution context stack overflow");
    }

    m_currentContext = &m_contextStack[m_contextIndex];
    m_currentContext->reset(scene, entity);
    m_currentContext->addOtherEntity(other);
}

void Game::pushExecutionContext(Scene* scene, Entity* entity, const std::vector<Entity*>& other_entities) {
    m_contextIndex++;

    if (m_contextIndex >= m_contextStack.size()) {
        raiseError("Execution context stack overflow");
    }

    m_currentContext = &m_contextStack[m_contextIndex];
    m_currentContext->reset(scene, entity);
    m_currentContext->setOtherEntities(other_entities);
}

void Game::popExecutionContext() {
    m_contextIndex--;

    if (m_contextIndex < 0) {
        raiseError("Execution context stack underflow");
    }

    m_currentContext = &m_contextStack[m_contextIndex];
}

GameExecutionContext* Game::getCurrentContext() {
//    return &m_context_stack[m_context_index];
    return m_currentContext;
}

void Game::setLanguage(const NomadString& languageCode) {
    m_language = languageCode;

    loadTextIntoConst(languageCode);
}

const NomadString& Game::getLanguage() const {
    return m_language;
}

NomadString& Game::getText(const NomadString& key, NomadString& text) const {
    m_resourceManager->getText()->getText(m_language, key, text);

    return text;
}

void Game::runInitScript() {
    log::info("Executing `init` script");

    NomadId init_script_id = m_runtime->getScriptId("init");

    if (init_script_id == NOMAD_INVALID_ID) {
        const char* message = "Fatal: No `init` script found";

        log::fatal(message);

        exit(EXIT_FAILURE);
    }

    try {
        executeScriptInCurrentContext(init_script_id);
    } catch (std::exception& e) {
        raiseError(NomadString("Failed to execute 'init' script: ") + e.what());
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

    for (auto added_scene: m_addedScenes) {
        m_scenes.push_back(added_scene);
    }
    m_addedScenes.clear();

    for (auto scene: m_removedScenes) {
        const auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
        if (it != m_scenes.end()) {
            m_scenes.erase(it);
        }
    }
    m_removedScenes.clear();

    // Sort scenes by layer
    std::sort(m_scenes.begin(), m_scenes.end(), [](Scene* a, Scene* b) {
        return a->getZ() < b->getZ();
    });

    resetTempHeap();
}

bool Game::processSystemInput(SDL_KeyboardEvent& event) {
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
                if (m_debugConsole == nullptr) {
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

void Game::processInput(SDL_KeyboardEvent& event) {
    // Process system input.
    if (processSystemInput(event)) {
        return;
    }

    InputEvent input_event;

    mapSdlKeyboardEvent(event, input_event);

    dispatchInputEvent(input_event);
}

void Game::processInput(SDL_MouseButtonEvent& event) {
    if (event.button <= m_mouseButtonState.size()) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                m_mouseButtonState[event.button].pressed = true;
                m_mouseButtonState[event.button].held = true;
                break;

            case SDL_MOUSEBUTTONUP:
                m_mouseButtonState[event.button].released = true;
                m_mouseButtonState[event.button].held = false;
                break;
        }
    }

    InputEvent input_event;

    mapSdlMouseEvent(event, input_event);

    dispatchInputEvent(input_event);
}

void Game::processInput(SDL_ControllerButtonEvent& event) const {
    InputEvent input_event;

    mapSdlControllerButtonEvent(event, input_event);

    dispatchInputEvent(input_event);
}

void Game::dispatchInputEvent(const InputEvent& event) const {
    for (const auto& scene: m_scenes) {
        scene->processInputEvent(event);
    }
}

Scene* Game::getCurrentScene() {
    return getCurrentContext()->getScene();
}

} // nomad