//
// Created by jfbil on 2023-06-07.
//

#pragma once

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
class ThisEntityVariableContext;
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

    [[nodiscard]] Runtime* getRuntime() const;
    [[nodiscard]] Canvas* getCanvas();
    [[nodiscard]] ResourceManager* getResources() const;

    void setDebug(bool debug);
    [[nodiscard]] bool isDebug() const;

    [[nodiscard]] const VariableMap* getSceneVariables() const;
    [[nodiscard]] const VariableMap* getEntityVariables() const;

    [[nodiscard]] const Point& getResolution() const;
    void setFps(NomadInteger fps);
    [[nodiscard]] NomadInteger getFps() const;

    [[nodiscard]] const NomadString& getOrganization() const;
    void setOrganization(const NomadString& organization);

    [[nodiscard]] const NomadString& getName() const;
    void setName(const NomadString& name);

    [[nodiscard]] NomadString getResourcePath() const;
    [[nodiscard]] NomadString getStatePath() const;
    [[nodiscard]] NomadString getSavePath() const;
    [[nodiscard]] NomadString getSettingsPath() const;

    [[nodiscard]] NomadString makeResourcePath(const NomadString& resourceName) const;
    [[nodiscard]] NomadString makeStatePath(const NomadString& stateName) const;
    [[nodiscard]] NomadString makeSavePath(const NomadString& saveName) const;
    [[nodiscard]] NomadString makeSettingsPath(const NomadString& settingsName) const;

    [[nodiscard]] bool fileExists(const NomadString& fileName) const;
    void deleteFile(const NomadString& fileName) const;

    void run();
    void quit();
    [[nodiscard]] uint64_t getUpdateDuration() const;
    [[nodiscard]] uint64_t getRenderDuration() const;

    [[nodiscard]] NomadId getScriptId(const NomadString& scriptName);
    void executeScriptInCurrentContext(NomadId scriptId);
    void executeScriptInCurrentContext(NomadId scriptId, ScriptValue& returnValue);
    void executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity);
    void executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, Entity* other);
    void executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, const std::vector<Entity*>& others);
    void executeScriptInNewContext(NomadId scriptId, Scene* scene, Entity* entity, ScriptValue& returnValue);
    void executeScriptInContext(NomadId scriptId, GameExecutionContext* context);
    void executeScriptInContext(NomadId scriptId, GameExecutionContext* context, ScriptValue& returnValue);
    void executeScriptByName(const NomadString& scriptName, Scene* scene, Entity* entity, ScriptValue& returnValue);

    [[nodiscard]] bool executePredicate(NomadId script_id);

    Scene* createScene();
    void removeScene(Scene* scene);

    [[nodiscard]] bool running() const;
    [[nodiscard]] bool isPaused() const;

    void pause();
    void resume();

    GameExecutionContext* getCurrentContext();

    void setLanguage(const NomadString& languageCode);
    [[nodiscard]] const NomadString& getLanguage() const;
    NomadString& getText(const NomadString& key, NomadString& text) const;

    [[noreturn]] void raiseError(const NomadString& message);

private:
    void initSdl();
    void initSdlTtf();
    void initResourcePath();
    void initRuntime();

    void initCommands();
    void initGameCommands();
    void initInputCommands();
    void initOtherEntityCommands();
    void initSceneCommands();
    void initThisEntityCommands();
    void initWindowCommands();
    void initVariableContext();
    void initResourceManager();
    void initConstants();
    void initLoadText();
    void initDebugConsole();

    // Init dynamic variables
    void initDynamicVariables();
    void initGameDynamicVariables();
    void initInputDynamicVariables();
    void initOtherDynamicVariables();
    void initSceneDynamicVariables();
    void initThisDynamicVariables();
    void initWindowDynamicVariables();

    void createPathToFile(const NomadString& fileName) const;
    void loadTextIntoConst(const NomadString& languageCode);

    void compileScripts();

    void pushExecutionContext(Scene* scene, Entity* entity);
    void pushExecutionContext(Scene* scene, Entity* entity, Entity* other);
    void pushExecutionContext(Scene* scene, Entity* entity, const std::vector<Entity*>& others);
    void popExecutionContext();
    void runInitScript();

    void render(Canvas* canvas);
    void update();

    void processInput(SDL_KeyboardEvent& event);
    bool processSystemInput(SDL_KeyboardEvent& event);
    void processInput(SDL_MouseButtonEvent& event);
    void processInput(SDL_ControllerButtonEvent& event) const;
    void dispatchInputEvent(const InputEvent& event) const;

    // The current scene being processed
    [[nodiscard]] Scene* getCurrentScene();

    NomadString m_title = "Nomad";
    Point m_resolution = {800, 600};
    Point m_windowSize = m_resolution;
    int m_fps = 30;

    const VariableMap* m_sceneVariableMap = nullptr;
    const VariableMap* m_thisEntityVariableMap = nullptr;
    const VariableMap* m_otherEntityVariableMap = nullptr;

    PointF m_mousePosition;
    struct MouseButtonState {
        bool pressed = false;
        bool held = false;
        bool released = false;
    };
    std::array<MouseButtonState, 3> m_mouseButtonState = {
        MouseButtonState{},
        MouseButtonState{},
        MouseButtonState{},
    };

    PointF m_mouseLastPosition;
    ResourceManager* m_resourceManager = nullptr;

    std::vector<Scene*> m_addedScenes;
    std::vector<Scene*> m_removedScenes;
    std::vector<Scene*> m_scenes;

    NomadString m_resourcePath;
    NomadString m_organization;
    NomadString m_name;

    bool m_debug = false;
    bool m_running = false;
    bool m_paused = false;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    Canvas* m_canvas = nullptr;

    // Performance
    uint64_t m_renderDuration = 0;
    uint64_t m_updateDuration = 0;

    InputManager* m_inputManager = nullptr;

    DebugConsole* m_debugConsole = nullptr;

    Runtime* m_runtime = nullptr;
    std::array<GameExecutionContext, 10> m_contextStack = {
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
    GameExecutionContext* m_currentContext = nullptr;
    int m_contextIndex = 0;

    NomadString m_language = "en";
};

} // nomad

