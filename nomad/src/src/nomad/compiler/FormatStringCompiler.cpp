//
// Created by jfbil on 2023-06-25.
//

#include "nomad/compiler/FormatStringCompiler.hpp"

#include "nomad/compiler/Compiler.hpp"

namespace nomad {

bool compileFormatString(Compiler* compiler, Script* script, const NomadString& format, FormatString* formatString) {
    NomadIndex index = 0;
    NomadIndex length = format.length();

    while (index < length) {
        NomadIndex start = index;

        while (index < length && format[index] != '{') {
            index++;
        }

        if (index > start) {
            formatString->addLiteral(format.substr(start, index - start));
        }

        if (index < length && format[index] == '{') {
            index++;
            start = index;

            auto end = start;

            while (end < length && format[end] != '}') {
                end++;
            }

            if (end >= length) {
                throw FormatStringException("Missing '}' in format string");
            }

            auto variableName = format.substr(start, end - start);

            IdentifierDefinition identifier{};

            compiler->getIdentifierDefinition(variableName, script, identifier);

            switch (identifier.identifierType) {
                case IdentifierType::Constant:
                case IdentifierType::Script:
                case IdentifierType::DynamicVariable:
                case IdentifierType::ContextVariable:
                case IdentifierType::ScriptVariable:
                case IdentifierType::Parameter:
                    // OK: Skip
                    break;

                case IdentifierType::Keyword:
                    throw FormatStringException("Cannot use keyword in format string: " + variableName);

                case IdentifierType::Command:
                    throw FormatStringException("Cannot use command in format string: " + variableName);

                case IdentifierType::Statement:
                    throw FormatStringException("Cannot use statement in format string: " + variableName);

                default:
                    throw FormatStringException("Unknown identifier in format string: " + variableName);
            }

            formatString->addVariable(
                identifier.identifierType,
                identifier.valueType,
                identifier.contextId,
                identifier.variableId
            );

            index = end + 1;
        }
    }

    return true;
}

} // nomad