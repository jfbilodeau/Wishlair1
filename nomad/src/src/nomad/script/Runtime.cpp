//
// Created by jfbil on 2023-06-04.
//

#include "nomad/script/Runtime.hpp"

#include "nomad/compiler/Compiler.hpp"

#include "nomad/script/Commands.hpp"
#include "nomad/script/Interpreter.hpp"
#include "nomad/script/Type.hpp"

#include <utility>

#include "boost/json/array.hpp"

#include <iomanip>

namespace nomad {

Runtime::Runtime() {
    // Types
    register_type(std::make_unique<VoidType>());
    register_type(std::make_unique<IdType>());
    register_type(std::make_unique<FloatType>());
    register_type(std::make_unique<IntegerType>());
    register_type(std::make_unique<BooleanType>());
    register_type(std::make_unique<StringType>());

    // Register instructions
    register_default_instructions();

    register_command(
        "log.info",
        command_log_info, {
            def_parameter("message", get_string_type(), NomadParamDoc("The text to print"))
        },
        get_void_type(),
        NomadDoc("Write a 'info' level string to the log.")
    );

    register_command(
        "toFloat",
        command_to_float, {
            def_parameter("value", get_integer_type(), NomadParamDoc("The integer to convert to a float"))
        },
        get_float_type(),
        NomadDoc("Convert an integer to a float.")
    );

    register_command(
        "toInteger",
        command_to_integer, {
            def_parameter("value", get_float_type(), NomadParamDoc("The float to convert to an integer"))
        },
        get_integer_type(),
        NomadDoc("Convert a float to an integer.")
    );

//    register_keyword("stringId" NomadDoc("Load the string id of the given string"));
    register_keyword("fun", NomadDoc("Declares a function"));
    register_keyword("else", NomadDoc("`else` branch of an `if` statement"));
    register_keyword("endIf", NomadDoc("End of an `if` statement"));
    register_keyword("end", NomadDoc("End of a `fun` statement"));

    register_constant("false", ScriptValue(NOMAD_FALSE), get_boolean_type());
    register_constant("true", ScriptValue(NOMAD_TRUE), get_boolean_type());
    register_constant("pi", ScriptValue(NOMAD_PI), get_float_type());

    register_variable_context("global", "global.", std::make_unique<SimpleVariableContext>());
}

Runtime::~Runtime() {
}

void Runtime::set_debug(bool debug) {
    m_debug = debug;
}

bool Runtime::is_debug() const {
    return m_debug;
}

const std::vector<Instruction>& Runtime::get_instructions() const {
    return m_instructions;
}

NomadId Runtime::register_command(
    const NomadString& name,
    CommandFn command_fn,
    const std::vector<CommandParameterDefinition>& parameters,
    const Type* return_type,
    NomadDocArg
) {
    NomadId id = get_command_id(name);

    if (id != NOMAD_INVALID_ID) {
        log::error("Command '" + name + "' already registered");

        return NOMAD_INVALID_ID;
    }

    id = to_nomad_id(m_commands.size());

    m_commands.push_back(
        {
            id,
            name,
            std::move(command_fn),
            parameters,
            return_type,
            doc
        }
    );

    return id;
}

NomadId Runtime::register_type(std::unique_ptr<Type>&& type) {
    m_types.push_back(std::move(type));

    return to_nomad_id(m_types.size() - 1);
}

const Type* Runtime::get_type(NomadId id) const {
    return m_types[id].get();
}

const Type* Runtime::get_type(const NomadString& name) const {
    for (const auto& type: m_types) {
        if (type->get_name() == name) {
            return type.get();
        }
    }

    return nullptr;
}

const Type* Runtime::get_callback_type(const std::vector<const Type*>& parameter_types, const Type* return_type) {
    auto test_type = std::make_unique<ScriptType>(parameter_types, return_type);

    for (const auto& type: m_types) {
        if (type->same_type(test_type.get())) {
            return type.get();
        }
    }

    m_types.push_back(std::move(test_type));

    return m_types.back().get();
}

const Type* Runtime::get_predicate_type() {
    return get_callback_type({}, get_boolean_type());
}

NomadId Runtime::register_instruction(
    const NomadString& name,
    InstructionFn fn,
    const NomadString& doc,
    std::vector<Operand> operands
) {
    auto id = get_instruction_id(name);

    if (id != NOMAD_INVALID_ID) {
        log::warning("Instruction '" + name + "' already registered");

        return id;
    }

    id = to_nomad_id(m_op_codes.size());

    m_op_codes.emplace_back(
        OpCodeDefinition{
            id,
            name,
            fn,
            std::move(operands),
            doc
        }
    );

    return id;
}

const Type* Runtime::get_void_type() const {
    return get_type(VOID_TYPE_NAME);
}
//
//const Type* Runtime::get_id_type() const {
//    return get_type(ID_TYPE_NAME);
//}

const Type* Runtime::get_boolean_type() const {
    return get_type(BOOLEAN_TYPE_NAME);
}

const Type* Runtime::get_integer_type() const {
    return get_type(INTEGER_TYPE_NAME);
}

const Type* Runtime::get_float_type() const {
    return get_type(FLOAT_TYPE_NAME);
}

const Type* Runtime::get_string_type() const {
    return get_type(STRING_TYPE_NAME);
}

const Type* Runtime::get_script_type() const {
    return get_type(SCRIPT_TYPE_NAME);
}

NomadId Runtime::get_instruction_id(const NomadString& name) const {
    for (auto i = 0; i < m_op_codes.size(); ++i) {
        if (m_op_codes[i].name == name) {
            return static_cast<NomadId>(i);
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Runtime::get_instruction_id(InstructionFn fn) const {
    for (auto& op_code: m_op_codes) {
        if (op_code.fn == fn) {
            return op_code.id;
        }
    }

    return NOMAD_INVALID_ID;
}

InstructionFn Runtime::get_instruction_fn(NomadId id) const {
    return m_op_codes[id].fn;
}

const NomadString& Runtime::get_instruction_name(NomadId id) const {
    return m_op_codes[id].name;
}

const std::vector<Operand>& Runtime::get_instruction_operands(NomadId id) const {
    return m_op_codes[id].operands;
}

NomadId Runtime::get_command_id(const NomadString& name) const {
    for (auto i = 0; i < m_commands.size(); ++i) {
        if (m_commands[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

CommandFn Runtime::get_command_fn(NomadId id) const {
    return m_commands[id].fn;
}

//NomadId Runtime::register_statement(const String& name, ParseStatementFn fn) {
//    auto id = to_nomad_id(m_statements.size());
//
//    ParseStatementFnRegistration registration;
//
//    registration.id = id;
//    registration.name = name;
//    registration.fn = fn;
//
//    m_statements.push_back(registration);
//
//    return id;
//}

//CompilerStatementFn Runtime::get_statement(const String& name) const {
//    for (const auto& statement: m_statements) {
//        if (statement.name == name) {
//            return statement.fn;
//        }
//    }
//
//    return nullptr;
//}
//
//void Runtime::get_statements(std::vector<CompilerStatementFnRegistration>& statements) const {
//    statements = m_statements;
//}
//
bool Runtime::get_command_definition(NomadId id, CommandDefinition& definition) const {
    if (id >= m_commands.size()) {
        return false;
    }

    definition.id = id;
    definition.name = m_commands[id].name;
    definition.fn = m_commands[id].fn;
    definition.parameters = m_commands[id].parameters;
    definition.return_type = m_commands[id].return_type;

    return true;
}

bool Runtime::get_command_definition(const NomadString& name, CommandDefinition& definition) const {
    auto id = get_command_id(name);

    if (id == NOMAD_INVALID_ID) {
        return false;
    }

    return get_command_definition(id, definition);
}

void Runtime::get_commands(std::vector<CommandDefinition>& commands) const {
    for (auto& command: m_commands) {
        CommandDefinition definition;

        definition.id = command.id;
        definition.name = command.name;
        definition.fn = command.fn;
        definition.parameters = command.parameters;
#if defined(NOMAD_SCRIPT_DOC)
        definition.doc = command.doc;
#endif

        commands.push_back(definition);
    }
}

NomadId Runtime::register_keyword(const NomadString& keyword, NomadDocArg) {
    auto id = to_nomad_id(m_keywords.size());

    KeywordDefinition definition;

    definition.keyword = keyword;
#if defined(NOMAD_SCRIPT_DOC)
    definition.doc = doc;
#endif

    m_keywords.push_back(definition);

    return id;
}

NomadId Runtime::get_keyword_id(const NomadString& keyword) const {
    for (auto i = 0; i < m_keywords.size(); ++i) {
        if (m_keywords[i].keyword == keyword) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

void Runtime::get_keywords(std::vector<KeywordDefinition>& keywords) const {
    keywords = m_keywords;
}

NomadId Runtime::register_constant(const NomadString& name, const ScriptValue& value, const Type* type) {
    if (get_constant_id(name) != NOMAD_INVALID_ID) {
        log::error("Constant with name '" + name + "' already registered.");

        return NOMAD_INVALID_ID;
    }

    auto constant_id = m_constants_map.register_variable(name, type);

    m_constants.resize(constant_id + 1);
    type->copy_value(value, m_constants[constant_id]);

    return constant_id;
}

NomadId Runtime::get_constant_id(const NomadString& name) const {
    return m_constants_map.get_variable_id(name);
}

void Runtime::get_constant_value(NomadId id, ScriptValue& value) const {
    value = m_constants[id];
}

const NomadString& Runtime::get_constant_name(NomadId id) const {
    return m_constants_map.get_variable_name(id);
}

const Type* Runtime::get_constant_type(NomadId id) const {
    return m_constants_map.get_variable_type(id);
}

NomadId Runtime::register_string(const NomadString& string) {
    auto id = get_string_id(string);

    if (id != NOMAD_INVALID_ID) {
        return id;
    }

    id = to_nomad_id(m_strings.size());

    m_strings.push_back(string);

    return id;
}

NomadId Runtime::get_string_id(const NomadString& string) const {
    for (auto i = 0; i < m_strings.size(); ++i) {
        if (m_strings[i] == string) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

const NomadString& Runtime::get_string(NomadId string_id) const {
    return m_strings[string_id];
}

const NomadString& Runtime::get_string_by_name(const NomadString& name) const {
    auto string_id = get_string_id(name);

    return get_string(string_id);
}

NomadId Runtime::register_format_string(const NomadString& format_string, NomadId script_id) {
    auto id = get_format_string_id(format_string, script_id);

    if (id != NOMAD_INVALID_ID) {
        return id;
    }

    id = to_nomad_id(m_format_strings.size());

    m_format_strings.push_back(
        {
            id,
            script_id,
            std::make_unique<FormatString>(format_string, script_id)
        }
    );

    return id;
}

FormatString* Runtime::get_format_string(NomadId id) const {
    if (id >= m_format_strings.size()) {
        return nullptr;
    }

    return m_format_strings[id].format_string.get();
}

NomadId Runtime::get_format_string_id(const NomadString& format_string, NomadId script_id) const {
    for (auto& i: m_format_strings) {
        if (i.script_id == script_id && i.format_string->get_format_string() == format_string) {
            return i.id;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadId Runtime::register_variable_context(
    const NomadString& name,
    const NomadString& prefix,
    std::unique_ptr<VariableContext> context
) {
    NomadId id = to_nomad_id(m_variables.size());

    auto registration = VariableContextRegistration{
        id,
        name,
        prefix,
        std::move(context)
    };

    m_variables.emplace_back(std::move(registration));

    return id;
}

NomadId Runtime::get_context_id(const NomadString& name) const {
    for (auto i = 0; i < m_variables.size(); ++i) {
        if (m_variables[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

VariableContext* Runtime::get_variable_context(NomadId context_id) const {
    if (context_id >= m_variables.size()) {
        return nullptr;
    }

    return m_variables[context_id].context.get();
}

NomadString Runtime::get_context_name(NomadId id) const {
    return m_variables[id].name;
}

NomadId Runtime::get_context_variable_id(NomadId context_id, const NomadString& variable_name) {
    auto variable_id = m_variables[context_id].context->get_variable_id(variable_name);

    if (variable_id == NOMAD_INVALID_ID) {
        variable_id = m_variables[context_id].context->register_variable(variable_name, nullptr);
    }

    return variable_id;
}

NomadId Runtime::get_variable_context_id_by_prefix(const NomadString& variable_name) const {
    for (const auto& context: m_variables) {
        if (variable_name.rfind(context.prefix, 0) == 0) {
            return context.id;
        }
    }

    return NOMAD_INVALID_ID;
}

NomadString Runtime::get_context_variable_name(NomadId context_id, NomadId variable_id) const {
    return m_variables[context_id].context->get_variable_name(variable_id);
}

const Type* Runtime::get_context_variable_type(NomadId context_id, NomadId variable_id) const {
    return m_variables[context_id].context->get_variable_type(variable_id);
}

void Runtime::set_context_variable_value(NomadId context_id, NomadId variable_id, const ScriptValue& value) const {
    m_variables[context_id].context->set_value(variable_id, value);
}

void Runtime::get_context_variable_value(NomadId context_id, NomadId variable_id, ScriptValue& value) const {
    m_variables[context_id].context->get_value(variable_id, value);
}

void
Runtime::set_string_context_variable_value(NomadId context_id, NomadId variable_id, const NomadString& value) const {
    ScriptValue string_value(value);
    m_variables[context_id].context->set_value(variable_id, string_value);
}

void Runtime::set_string_context_variable_value(NomadId context_id, NomadId variable_id, const NomadChar* value) const {
    ScriptValue string_value(value);
    m_variables[context_id].context->set_value(variable_id, string_value);
}

void Runtime::get_string_context_variable_value(NomadId context_id, NomadId variable_id, NomadString& value) const {
    ScriptValue string_value;
    m_variables[context_id].context->get_value(variable_id, string_value);

    value = string_value.get_string_value();
}

NomadId Runtime::register_script(
    const NomadString& name,
    const NomadString& path,
    const NomadString& source
) {
    if (get_script_id(name) != NOMAD_INVALID_ID) {
        log::error("Script with name '" + name + "' already registered.");

        return NOMAD_INVALID_ID;
    }

    auto id = to_nomad_id(m_scripts.size());

    auto script = new Script(id, name, path, source);

    m_scripts.push_back(script);

    return id;
}

NomadId Runtime::get_script_id(const NomadString& name) const {
    for (auto i = 0; i < m_scripts.size(); ++i) {
        if (m_scripts[i]->get_name() == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

Script* Runtime::get_script(NomadId script_id) const {
    return m_scripts[script_id];
}

NomadIndex Runtime::get_script_count() const {
    return m_scripts.size();
}

void Runtime::get_scripts(std::vector<Script*>& scripts) {
    scripts.reserve(m_scripts.size() + scripts.size());

    for (auto script: m_scripts) {
        scripts.push_back(script);
    }
}

NomadIndex Runtime::get_script_size() const {
    NomadIndex size = 0;

    for (const auto& script: m_scripts) {
        size += script->get_script_length();
    }

    return size;
}

std::unique_ptr<Compiler> Runtime::create_compiler() {
    return std::make_unique<Compiler>(this, m_instructions);
}

NomadId Runtime::register_dynamic_variable(
    const NomadString& name,
    DynamicVariableSetFn set_fn,
    DynamicVariableGetFn get_fn,
    const Type* type,
    NomadDocArg
) {
    auto id = to_nomad_id(m_dynamic_variables.size());

    DynamicVariableRegistration definition;
    definition.name = name;
    definition.type = type;
    definition.set_fn = std::move(set_fn);
    definition.get_fn = std::move(get_fn);
#if defined(NOMAD_SCRIPT_DOC)
    definition.doc = doc;
#endif

    m_dynamic_variables.push_back(definition);

    return id;
}

NomadId Runtime::get_dynamic_variable_id(const NomadString& name) const {
    for (auto i = 0; i < m_dynamic_variables.size(); ++i) {
        if (m_dynamic_variables[i].name == name) {
            return to_nomad_id(i);
        }
    }

    return NOMAD_INVALID_ID;
}

NomadString Runtime::get_dynamic_variable_name(NomadId id) const {
    return m_dynamic_variables[id].name;
}

const Type* Runtime::get_dynamic_variable_type(NomadId id) const {
    return m_dynamic_variables[id].type;
}

bool Runtime::can_set_dynamic_variable(NomadId id) const {
    return m_dynamic_variables[id].set_fn != nullptr;
}

bool Runtime::can_get_dynamic_variable(NomadId id) const {
    return m_dynamic_variables[id].get_fn != nullptr;
}

void Runtime::set_dynamic_variable(Interpreter* interpreter, NomadId id, const ScriptValue& value) {
    m_dynamic_variables[id].set_fn(interpreter, value);
}

void Runtime::get_dynamic_variable_value(Interpreter* interpreter, NomadId id, ScriptValue& value) {
    m_dynamic_variables[id].get_fn(interpreter, value);
}

void Runtime::set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const NomadString& value) {
    set_string_dynamic_variable(interpreter, id, value.c_str());
}

void Runtime::set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const NomadChar* value) {
    ScriptValue string_value(value);
    set_string_dynamic_variable(interpreter, id, string_value);
}

void Runtime::set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const ScriptValue& value) {
    m_dynamic_variables[id].set_fn(interpreter, value);
}

void Runtime::get_string_dynamic_variable_value(Interpreter* interpreter, NomadId id, NomadString& value) {
    ScriptValue string_value;

    get_string_dynamic_variable_value(interpreter, id, string_value);

    value = string_value.get_string_value();
}

void Runtime::get_string_dynamic_variable_value(Interpreter* interpreter, NomadId id, ScriptValue& value) {
    m_dynamic_variables[id].get_fn(interpreter, value);
}

//std::shared_ptr<Logger> Runtime::get_logger() const {
//    return m_logger;
//}
//
//void Runtime::get_opcode_definitions(std::vector<OpCodeDefinition>& definitions) const {
//    definitions = m_op_codes;
//}

std::unique_ptr<Interpreter> Runtime::create_interpreter(NomadId script_id) {
    auto script = m_scripts[script_id];

    return std::make_unique<Interpreter>(this);
}

void Runtime::execute_script(NomadId script_id) {
    if (m_interpreter == nullptr) {
        m_interpreter = std::make_unique<Interpreter>(this);
    }

    auto script = m_scripts[script_id];

     return m_interpreter->run(script, {});
}

void Runtime::execute_script(NomadId script_id, ScriptValue& return_value) {
    auto script = m_scripts[script_id];

    Interpreter interpreter(this);

    interpreter.run(script, {});

    return_value = interpreter.get_result();
}

void Runtime::dump_instructions(std::ostream& out) {
    NomadId script_id = 0;
    auto script = get_script(script_id);

    for (auto i = 0; i < m_instructions.size(); ++i) {
        // Should we move to the next script?
        if (i == script->get_script_end()) {
            script_id++;
            script = get_script(script_id);
        }

        auto fn = m_instructions[i].fn;

        auto instruction_id = get_instruction_id(fn);

        if (instruction_id == NOMAD_INVALID_ID) {
            out << std::setfill('0') << std::setw(4) << i << ": Unknown instruction" << std::endl;
            continue;
        }

        auto& name = get_instruction_name(instruction_id);
        auto& operands = get_instruction_operands(instruction_id);

        out << std::setfill('0') << std::setw(4) << i << ": " << name << std::endl;

        for (auto operand : operands) {
            i++;

            auto& value = m_instructions[i].value;

            NomadString text_value;

            switch (operand) {
                case Operand::Boolean:
                    text_value = value.get_boolean_value() ? "true" : "false";
                    break;
                case Operand::Integer:
                    text_value = std::to_string(value.get_integer_value());
                    break;
                case Operand::Float:
                    text_value = std::to_string(value.get_float_value());
                    break;
                case Operand::String:
                    text_value = "\"" + get_string(value.get_id_value()) + "\"";
                    break;
                case Operand::Id:
                    text_value = std::to_string(value.get_id_value());
                    break;
                case Operand::Script:
                    text_value = get_script(value.get_id_value())->get_name();
                    break;
                case Operand::Command: {
                    CommandDefinition definition;
                    auto result = get_command_definition(value.get_id_value(), definition);

                    if (result) {
                        text_value = definition.name;
                    } else {
                        text_value = "<unknown command>";
                    }
                    break;
                }
                case Operand::ScriptVariable:
                    text_value = script->get_variable_name(value.get_id_value());
                    break;
                case Operand::DynamicVariable:
                    text_value = get_dynamic_variable_name(value.get_id_value());
                    break;
                case Operand::ContextId: {
                    auto context_id = value.get_id_value();
                    auto variable_id = m_instructions[i+1].value.get_id_value();
                    text_value = get_context_variable_name(context_id, variable_id);
                    break;
                }
                case Operand::ContextVariableId:
                    continue; // Skip over context variable id
                    log::debug("[Runtime::get_operand_text] ContextVariableId operand type should have already been handled");
                    break;
                case Operand::FormatString:
                    text_value = "$\"" + get_format_string(value.get_id_value())->get_format_string() + "\"";
                    break;
                default:
                    text_value = "unknown";
                    log::debug("[Runtime::get_operand_text] Unknown operand type: " + std::to_string(static_cast<int>(operand)));
            }

            out
                << std::setfill('0')
                << std::setw(4)
                << i
                << ": "
                << text_value
                << std::endl;
        }

        out.flush();
    }
}

void Runtime::dump_documentation(std::ostream &out) {
    // Output TOC
    out << "# Nomad Engine Reference" << std::endl;

    out << "* [Constants](#Constants)" << std::endl;
    out << "* [Variable contexts](#Variable-contexts)" << std::endl;
    out << "* [Commands](#Commands)" << std::endl;
    out << "* [Instructions](#Instructions)" << std::endl;

    out << "---" << std::endl;

    out << "## Constants" << std::endl;

    for (auto i = 0; i < m_constants_map.get_variable_count(); ++i) {
        ScriptValue constant_value;
        NomadString constant_text_value;

        auto constant_type = get_constant_type(i);
        get_constant_value(i, constant_value);

        if (constant_type == get_void_type()) {
            constant_text_value = "";
        } else if (constant_type == get_boolean_type()) {
            constant_text_value = constant_value.get_boolean_value() ? "true" : "false";
        } else if (constant_type == get_integer_type()) {
            constant_text_value = std::format("{0} ({0:#0x})", constant_value.get_integer_value());
        } else if (constant_type == get_float_type()) {
            constant_text_value = std::format("{:}", constant_value.get_float_value());
        } else if (constant_type == get_string_type()) {
            constant_text_value = std::format("\"{}\"", constant_value.get_string_value());
        } else if (constant_type == get_script_type()) {
            constant_text_value = get_script(constant_value.get_id_value())->get_name();
        } else {
            constant_text_value = "<unknown_type>";
        }

        out
            << "* `"
            << get_constant_name(i)
            << ":"
            << get_constant_type(i)->get_name()
            << " = "
            << constant_text_value
            << "`"
            << std::endl;
    }

    out << std::endl;

    out << "## Variable contexts" << std::endl;

    for (auto& context: m_variables) {
        out << "- `" << context.prefix << "`" << " (" << context.name << ")" << std::endl;
    }

    out << std::endl;

    out << "## Commands" << std::endl;

    for (auto& command: m_commands) {
        out << "`" << command.name << "`" << std::endl;

        for (auto& parameter: command.parameters) {
            out << "- `" << parameter.name << ":" << parameter.type->get_name() << "`" << std::endl;
        }

        out << std::endl;

        out << "`return " << command.return_type->get_name() << "`" << std::endl;

        out << std::endl;

        out << command.doc << std::endl;
        out << std::endl;
        out << "---" << std::endl;
    }

    out << std::endl;

    out << "## Instructions" << std::endl;

    for (auto& op_code: m_op_codes) {
        out << "`" << op_code.name << "`" << std::endl;
        out << op_code.doc << std::endl;
        out << std::endl;
    }

    out.flush();
}

} // namespace nomad