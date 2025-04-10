//
// Created by jfbil on 2023-06-13.
//

#include <cmath>

#include "nomad/script/Commands.hpp"

#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Runtime.hpp"
#include "nomad/system/Float.hpp"
#include "nomad/system/Integer.hpp"

namespace nomad {

void registerBuildInCommands(Runtime *runtime) {
    runtime->registerCommand(
        "log.info",
        [](Interpreter* interpreter) {
            auto message = interpreter->getStringParameter(0);

            TempString scriptName = "<unknown";
            TempString fileName = "<unknown>";

            auto script = interpreter->getCurrentScript();

            if (script) {
                scriptName = script->getName();
                fileName = script->getPath();
            }

            Location location(
                scriptName.c_str(),
                fileName.c_str(),
                0,
                0
            );

           log::info(message, location);
        },  // Command function
        {
            defParameter("message", runtime->getStringType(), NomadParamDoc("The text to print"))
        },
        runtime->getVoidType(),
        NomadDoc("Write a 'info' level string to the log.")
    );

    runtime->registerCommand(
        "math.floor",
        [](Interpreter* interpreter) {
            auto value = interpreter->getFloatParameter(0);

            auto floored_value = floatFloor(value);

            interpreter->setFloatResult(floored_value);
        },  // Command function
        {
            defParameter("value", runtime->getFloatType(), NomadParamDoc("The float to floor"))
        },
        runtime->getFloatType(),
        NomadDoc("Floor a float value.")
    );

    runtime->registerCommand(
        "toFloat",
        [](Interpreter* interpreter) {
            auto value = interpreter->getIntegerParameter(0);

            auto converted_value = integerToFloat(value);

            interpreter->setFloatResult(converted_value);
        },  // Command function
        {
            defParameter("value", runtime->getIntegerType(), NomadParamDoc("The integer to convert to a float"))
        },
        runtime->getFloatType(),
        NomadDoc("Convert an integer to a float.")
    );

    runtime->registerCommand(
        "toInteger",
        [](Interpreter* interpreter) {
            auto value = interpreter->getFloatParameter(0);

            auto converted_value = floatToInteger(value);

            interpreter->setIntegerResult(converted_value);
        },
        {
            defParameter("value", runtime->getFloatType(), NomadParamDoc("The float to convert to an integer"))
        },
        runtime->getIntegerType(),
        NomadDoc("Convert a float to an integer.")
    );
}

} // nomad