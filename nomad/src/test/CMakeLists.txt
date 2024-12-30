cmake_minimum_required(VERSION 3.20)
project(cpp)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

include_directories(${doctest_SOURCE_DIR}/doctest)

add_executable(
        nomad_test

#        doctest.h
        test_main.cpp

        Compiler.Test.cpp
        FormatString.Test.cpp
        Interpreter.Test.cpp
        Parser.Test.cpp

        ${NOMAD_SRC}
        ${NOMAD_INCLUDE}
)

target_link_libraries(
        nomad_test
        ${NOMAD_LIBRARIES}
)
add_custom_command(TARGET nomad_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nomad_test> $<TARGET_FILE_DIR:nomad_test>
        COMMAND_EXPAND_LISTS
        )

