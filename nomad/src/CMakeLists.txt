cmake_minimum_required(VERSION 3.24)  # Need 3.24 for FetchContent_MakeAvailable

project(nomad CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

# Disable installation of libraries
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
#set(SDL2IMAGE_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE)
#set(SDL2IMAGE_DEPS_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2NET_INSTALL OFF CACHE BOOL "" FORCE)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        # Need to stick with 1.80 until following issue is addressed: https://github.com/boostorg/phoenix/issues/111
        GIT_TAG boost-1.80.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

#FetchContent_Declare(
#        SDL2
#        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#        GIT_TAG release-2.30.10
#        GIT_SHALLOW    TRUE
#        GIT_PROGRESS   TRUE
#        OVERRIDE_FIND_PACKAGE
#)

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.10
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.3
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-2.20.2
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
        DearImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.89.9-docking
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d
        GIT_TAG v3.0.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(Boost SDL2 SDL2_image SDL2_ttf DearImGui)

find_package(Boost REQUIRED json)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(DearImGui REQUIRED)
find_package(box2d REQUIRED)

set(
        DEAR_IMGUI_SOURCE

        ${dearimgui_SOURCE_DIR}/imgui.cpp
        ${dearimgui_SOURCE_DIR}/imgui_draw.cpp
        ${dearimgui_SOURCE_DIR}/imgui_tables.cpp
        ${dearimgui_SOURCE_DIR}/imgui_widgets.cpp

        ${dearimgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${dearimgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
set(
        DEAR_IMGUI_INCLUDE_DIRS
        ${dearimgui_SOURCE_DIR}
)

# Path to apps (main())
set(NOMAD_APP_DIR ${CMAKE_SOURCE_DIR}/app)

# Path to nomad sources
set(NOMAD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(NOMAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(
        ${DEAR_IMGUI_INCLUDE_DIRS}
        ${NOMAD_INCLUDE_DIR}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(NOMAD_DEBUG)
    add_compile_definitions(NOMAD_SCRIPT_DOC)
endif()

# Warnings are good. Errors are better.
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(
        NOMAD_LIBRARIES
        ${NOMAD_LIBRARIES}

        Boost::json
        Boost::program_options
        Boost::spirit

        SDL2::SDL2main
        SDL2::SDL2

        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf

        box2d
)

set(
        NOMAD_INCLUDE

        ${DEAR_IMGUI_INCLUDE}

        ${NOMAD_INCLUDE_DIR}/nomad/Nomad.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Argument.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Compiler.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/CompilerException.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Expression.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/FormatStringCompiler.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Identifier.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Operators.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Parser.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/StatementParsers.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/SyntaxTree.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/compiler/Tokenizer.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/compiler/statements/ReturnStatement.hpp

#        ${NOMAD_INCLUDE_DIR}/nomad/debug/Decompiler.hpp
#        ${NOMAD_INCLUDE_DIR}/nomad/debug/DebugConsole.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/game/Alignment.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/ActionManager.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Canvas.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Color.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Entity.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/EntityVariableContext.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Event.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Game.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/GameExecutionContext.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/GameFactory.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/GameStatementParsers.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/InputManager.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Layer.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/Scene.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/System.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/game/TileMap.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/geometry/Circle.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/CircleF.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/Geometry.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/Intersection.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/Point.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/PointF.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/Rectangle.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/geometry/RectangleF.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/log/ConsoleSink.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/log/Logger.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/log/MemorySink.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/resource/Animation.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/Font.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/Resource.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/ResourceManager.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/Sprite.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/SpriteAtlas.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/Texture.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/resource/Text.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/script/Commands.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Documentation.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/DynamicVariable.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/FormatString.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Instructions.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Interpreter.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/OpCode.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Runtime.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Script.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/ScriptValue.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Type.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/Variable.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/script/VariableContext.hpp

        ${NOMAD_INCLUDE_DIR}/nomad/system/Boolean.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/system/FastHeap.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/system/Float.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/system/Integer.hpp
        ${NOMAD_INCLUDE_DIR}/nomad/system/String.hpp
)

set(
        NOMAD_SRC

        ${DEAR_IMGUI_SOURCE}

        ${NOMAD_SOURCE_DIR}/nomad/compiler/Argument.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/Compiler.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/Expression.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/FormatStringCompiler.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/Operators.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/Parser.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/StatementParsers.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/SyntaxTree.cpp
        ${NOMAD_SOURCE_DIR}/nomad/compiler/Tokenizer.cpp

        ${NOMAD_SOURCE_DIR}/nomad/compiler/statements/ReturnStatement.cpp

#        ${NOMAD_SOURCE_DIR}/nomad/debug/Decompiler.cpp
#        ${NOMAD_SOURCE_DIR}/nomad/debug/DebugConsole.cpp

        ${NOMAD_SOURCE_DIR}/nomad/game/Alignment.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/ActionManager.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Canvas.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Color.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Entity.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/EntityVariableContext.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Event.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Game.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/GameExecutionContext.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/GameFactory.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/GameStatementParsers.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/InputManager.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Layer.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Scene.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Scene.load_tile_map.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Scene.render.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/Scene.update.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/System.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/TileMap.cpp

        ${NOMAD_SOURCE_DIR}/nomad/game/commands/EntityCommands.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/commands/GameCommands.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/commands/InputCommands.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/commands/SceneCommands.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/commands/WindowCommands.cpp

        ${NOMAD_SOURCE_DIR}/nomad/game/variables/GameDynamicVariables.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/variables/InputDynamicVariables.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/variables/OtherDynamicVariables.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/variables/SceneDynamicVariables.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/variables/ThisDynamicVariables.cpp
        ${NOMAD_SOURCE_DIR}/nomad/game/variables/WindowDynamicVariables.cpp

        ${NOMAD_SOURCE_DIR}/nomad/geometry/Circle.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/CircleF.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/Point.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/Intersection.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/PointF.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/Rectangle.cpp
        ${NOMAD_SOURCE_DIR}/nomad/geometry/RectangleF.cpp

        ${NOMAD_SOURCE_DIR}/nomad/log/ConsoleSink.cpp
        ${NOMAD_SOURCE_DIR}/nomad/log/Logger.cpp
        ${NOMAD_SOURCE_DIR}/nomad/log/MemorySink.cpp

        ${NOMAD_SOURCE_DIR}/nomad/resource/Animation.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/Font.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/Resource.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/ResourceManager.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/Sprite.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/SpriteAtlas.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/Texture.cpp
        ${NOMAD_SOURCE_DIR}/nomad/resource/Text.cpp

        ${NOMAD_SOURCE_DIR}/nomad/script/Command.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Commands.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Documentation.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/DynamicVariable.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/FormatString.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Instructions.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Interpreter.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/OpCode.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Runtime.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/RuntimeOpCodes.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Script.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/ScriptValue.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Type.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/Variable.cpp
        ${NOMAD_SOURCE_DIR}/nomad/script/VariableContext.cpp

        ${NOMAD_SOURCE_DIR}/nomad/system/FastHeap.cpp
        ${NOMAD_SOURCE_DIR}/nomad/system/String.cpp
)

# Nomad game engine
add_executable(
        nomad

        ${NOMAD_APP_DIR}/nomad/main.cpp

        ${NOMAD_SRC}
        ${NOMAD_INCLUDE}
)

target_link_libraries(nomad ${NOMAD_LIBRARIES})

# Only execute on Windows.
if(WIN32)
add_custom_command(TARGET nomad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nomad> $<TARGET_FILE_DIR:nomad>
        COMMAND_EXPAND_LISTS
)
endif()

# Copy licenses to the output directory
configure_file(
        ${FETCHCONTENT_BASE_DIR}/boost-src/LICENSE_1_0.txt
        ${nomad_BINARY_DIR}/licenses/BOOST_LICENSE_1_0.txt
        COPYONLY
)
configure_file(
        ${FETCHCONTENT_BASE_DIR}/box2d-src/LICENSE
        ${nomad_BINARY_DIR}/licenses/BOX2D_LICENSE
        COPYONLY
)
configure_file(
        ${FETCHCONTENT_BASE_DIR}/dearimgui-src/LICENSE.txt
        ${nomad_BINARY_DIR}/licenses/DEARIMGUI_LICENSE.txt
        COPYONLY
)
configure_file(
        ${FETCHCONTENT_BASE_DIR}/sdl2-src/LICENSE.txt
        ${nomad_BINARY_DIR}/licenses/SDL2_LICENSE.txt
        COPYONLY
)
configure_file(
        ${FETCHCONTENT_BASE_DIR}/sdl2_image-src/LICENSE.txt
        ${nomad_BINARY_DIR}/licenses/SDL2_IMAGE_LICENSE.txt
        COPYONLY
)
configure_file(
        ${FETCHCONTENT_BASE_DIR}/sdl2_ttf-src/LICENSE.txt
        ${nomad_BINARY_DIR}/licenses/SDL2_TTF_LICENSE.txt
        COPYONLY
)

# Copy Wishlair license
configure_file(
        ${CMAKE_SOURCE_DIR}/LICENSE.txt
        ${nomad_BINARY_DIR}/LICENSE.txt
        COPYONLY
)