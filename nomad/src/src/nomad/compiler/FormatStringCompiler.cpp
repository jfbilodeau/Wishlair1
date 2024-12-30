//
// Created by jfbil on 2023-06-25.
//

#include "nomad/compiler/FormatStringCompiler.hpp"

#include "nomad/compiler/Compiler.hpp"

namespace nomad {

bool compile_format_string(Compiler* compiler, Script* script, const NomadString& format, FormatString* format_string) {
    NomadIndex index = 0;
    NomadIndex length = format.length();

    while (index < length) {
        NomadIndex start = index;

        while (index < length && format[index] != '{') {
            index++;
        }

        if (index > start) {
            format_string->add_literal(format.substr(start, index - start));
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

            auto variable_name = format.substr(start, end - start);

            IdentifierDefinition identifier{};

            compiler->get_identifier_definition(variable_name, script, identifier);

            switch (identifier.identifier_type) {
                case IdentifierType::Constant:
                case IdentifierType::Script:
                case IdentifierType::DynamicVariable:
                case IdentifierType::ContextVariable:
                case IdentifierType::ScriptVariable:
                case IdentifierType::Parameter:
                    // OK: Skip
                    break;

                case IdentifierType::Keyword:
                    throw FormatStringException("Cannot use keyword in format string: " + variable_name);

                case IdentifierType::Command:
                    throw FormatStringException("Cannot use command in format string: " + variable_name);

                case IdentifierType::Statement:
                    throw FormatStringException("Cannot use statement in format string: " + variable_name);

                default:
                    throw FormatStringException("Unknown identifier in format string: " + variable_name);
            }

            format_string->add_variable(
                identifier.identifier_type,
                identifier.value_type,
                identifier.context_id,
                identifier.variable_id
            );

            index = end + 1;
        }
    }

    return true;
}

} // nomad