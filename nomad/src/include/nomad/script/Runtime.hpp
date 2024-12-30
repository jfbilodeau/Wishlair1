//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_RUNTIME_RUNTIME_HPP
#define NOMAD_RUNTIME_RUNTIME_HPP

#include "nomad/Nomad.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/Documentation.hpp"
#include "nomad/script/DynamicVariable.hpp"
#include "nomad/script/Script.hpp"
#include "nomad/script/VariableContext.hpp"

namespace nomad {

// Forward declarations
class Compiler;
class FormatString;
class Interpreter;
class Parser;
class ScriptBuilder;
class Tokenizer;
class Type;

enum class Operand {
    Boolean,
    Integer,
    Float,
    String,
    Id,
    Script,
    Command,
    ScriptVariable,
    DynamicVariable,
    ContextId,
    ContextVariableId,
    FormatString,
};

//using CompilerStatementFn = void (*)(Compiler*, Tokenizer*, ScriptBuilder*);
struct OpCodeDefinition {
    NomadId id;
    NomadString name;
    InstructionFn fn;
    std::vector<Operand> operands;
    NomadDocField;
};

struct KeywordDefinition {
    NomadString keyword;
    NomadDocField;
};

class Runtime {
private:
    struct DynamicVariableRegistration {
        NomadString name;
        const Type *type;
        DynamicVariableSetFn set_fn;
        DynamicVariableGetFn get_fn;
        NomadDocField;
    };

    struct ContextVariable {
        NomadString name;
        Type* type;
        ScriptValue value;
    };

    struct VariableContextRegistration {
        NomadId id;
        NomadString name;
        NomadString prefix;
        std::vector<NomadString> variables;
        std::unique_ptr<VariableContext> context;
    };

public:
    Runtime();
    Runtime(const Runtime&) = delete;
    ~Runtime();

    // Generate opcodes that are easier to debug
    // ie: replace op_load_constant with op_load_value
    void set_debug(bool debug);
    [[nodiscard]] bool is_debug() const;

    // Instructions
    [[nodiscard]] const std::vector<Instruction>& get_instructions() const;

    // Types
    NomadId register_type(std::unique_ptr<Type>&& type);
    [[nodiscard]] const Type* get_type(NomadId id) const;
    [[nodiscard]] const Type* get_type(const NomadString& name) const;
    [[nodiscard]] const Type* get_callback_type(const std::vector<const Type*>& parameter_types, const Type* return_type);
    [[nodiscard]] const Type* get_predicate_type();

    // Convenience type access:
    [[nodiscard]] const Type* get_void_type() const;
//    [[nodiscard]] const Type* get_id_type() const;
    [[nodiscard]] const Type* get_boolean_type() const;
    [[nodiscard]] const Type* get_integer_type() const;
    [[nodiscard]] const Type* get_float_type() const;
    [[nodiscard]] const Type* get_string_type() const;
    [[nodiscard]] const Type* get_script_type() const;

    // OpCodes
    NomadId register_instruction(const NomadString& name, InstructionFn fn, NomadDocArg, std::vector<Operand> operands);
    [[nodiscard]] NomadId get_instruction_id(const NomadString& name) const;
    [[nodiscard]] NomadId get_instruction_id(InstructionFn fn) const;
    [[nodiscard]] InstructionFn get_instruction_fn(NomadId id) const;
    [[nodiscard]] const NomadString& get_instruction_name(NomadId id) const;
    [[nodiscard]] const std::vector<Operand>& get_instruction_operands(NomadId id) const;

    // Commands
    NomadId register_command(const NomadString& name, CommandFn command_fn, const std::vector<CommandParameterDefinition>& parameters, const Type* return_type, NomadDocArg);
    [[nodiscard]] NomadId get_command_id(const NomadString& name) const;
    [[nodiscard]] CommandFn get_command_fn(NomadId id) const;

    bool get_command_definition(NomadId id, CommandDefinition& definition) const;
    bool get_command_definition(const NomadString& name, CommandDefinition& definition) const;
    void get_commands(std::vector<CommandDefinition>& commands) const;

    // Keywords
    NomadId register_keyword(const NomadString& keyword, NomadDocArg);
    [[nodiscard]] NomadId get_keyword_id(const NomadString& keyword) const;
    void get_keywords(std::vector<KeywordDefinition>& keywords) const;

    // Constants
    NomadId register_constant(const NomadString& name, const ScriptValue& value, const Type* type);
    void get_constant_value(NomadId id, ScriptValue& value) const;
    [[nodiscard]] NomadId get_constant_id(const NomadString& name) const;
    [[nodiscard]] const NomadString& get_constant_name(NomadId id) const;
    [[nodiscard]] const Type* get_constant_type(NomadId id) const;

    // Static strings
    NomadId register_string(const NomadString& string);
    [[nodiscard]] NomadId get_string_id(const NomadString& string) const;
    [[nodiscard]] const NomadString& get_string(NomadId id) const;
    [[nodiscard]] const NomadString& get_string_by_name(const NomadString& name) const;

    // Format strings
    NomadId register_format_string(const NomadString& format_string, NomadId script_id);
    [[nodiscard]] FormatString* get_format_string(NomadId id) const;
    [[nodiscard]] NomadId get_format_string_id(const NomadString& format_string, NomadId script_id) const;

    // Dynamic variables
    NomadId register_dynamic_variable(const NomadString& name, DynamicVariableSetFn set_fn, DynamicVariableGetFn get_fn, const Type* type, NomadDocArg);
    [[nodiscard]] NomadId get_dynamic_variable_id(const NomadString& name) const;
    [[nodiscard]] NomadString get_dynamic_variable_name(NomadId id) const;
    [[nodiscard]] const Type* get_dynamic_variable_type(NomadId id) const;
    [[nodiscard]] bool can_set_dynamic_variable(NomadId id) const;
    [[nodiscard]] bool can_get_dynamic_variable(NomadId id) const;
    void set_dynamic_variable(Interpreter* interpreter, NomadId id, const ScriptValue& value);
    void get_dynamic_variable_value(Interpreter* interpreter, NomadId id, ScriptValue& value);
    void set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const NomadString& value);
    void set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const NomadChar* value);
    void set_string_dynamic_variable(Interpreter* interpreter, NomadId id, const ScriptValue& value);

    void get_string_dynamic_variable_value(Interpreter* interpreter, NomadId id, NomadString& value);
    void get_string_dynamic_variable_value(Interpreter* interpreter, NomadId id, ScriptValue& value);

    // Variable contexts
    NomadId register_variable_context(const NomadString& name, const NomadString& prefix, std::unique_ptr<VariableContext> context);
    [[nodiscard]] NomadId get_context_id(const NomadString& name) const;
    [[nodiscard]] VariableContext* get_variable_context(NomadId context_id) const;
    [[nodiscard]] NomadString get_context_name(NomadId id) const;
    NomadId get_context_variable_id(NomadId context_id, const NomadString& variable_name);
    [[nodiscard]] NomadId get_variable_context_id_by_prefix(const NomadString& variable_name) const;
    [[nodiscard]] NomadString get_context_variable_name(NomadId context_id, NomadId variable_id) const;
    [[nodiscard]] const Type* get_context_variable_type(NomadId context_id, NomadId variable_id) const;
    void set_context_variable_value(NomadId context_id, NomadId variable_id, const ScriptValue& value) const;
    void get_context_variable_value(NomadId context_id, NomadId variable_id, ScriptValue& value) const;
    void set_string_context_variable_value(NomadId context_id, NomadId variable_id, const NomadString& value) const;
    void set_string_context_variable_value(NomadId context_id, NomadId variable_id, const NomadChar* value) const;
    void get_string_context_variable_value(NomadId context_id, NomadId variable_id, NomadString& value) const;

    // Scripts
    NomadId register_script(const NomadString& name, const NomadString& path, const NomadString& source);
    [[nodiscard]] NomadId get_script_id(const NomadString& name) const;
    [[nodiscard]] Script* get_script(NomadId script_id) const;
    [[nodiscard]] NomadIndex get_script_count() const;
    void get_scripts(std::vector<Script*>& scripts);
    [[nodiscard]] NomadIndex get_script_size() const;

    std::unique_ptr<Compiler> create_compiler();

    std::unique_ptr<Interpreter> create_interpreter(NomadId script_id);
    void execute_script(NomadId script_id);
    void execute_script(NomadId script_id, ScriptValue& return_value);

    // Debug
    void dump_instructions(std::ostream& out);

private:
    struct FormatStringRegistration {
        NomadId id;
        NomadId script_id;
        std::unique_ptr<FormatString> format_string;
    };

    void register_default_instructions();

    std::vector<Instruction> m_instructions;
    std::vector<OpCodeDefinition> m_op_codes;
    std::vector<std::unique_ptr<Type>> m_types;
    std::vector<CommandDefinition> m_commands;
    std::vector<KeywordDefinition> m_keywords;
    VariableMap m_constants_map;
    std::vector<ScriptValue> m_constants;
    std::vector<NomadString> m_strings;
    std::vector<DynamicVariableRegistration> m_dynamic_variables;
    std::vector<VariableContextRegistration> m_variables;
    std::vector<FormatStringRegistration> m_format_strings;
    std::vector<Script*> m_scripts;
    bool m_debug = false;
    std::unique_ptr<Interpreter> m_interpreter = nullptr;
};
} // nomad

#endif //NOMAD_RUNTIME_RUNTIME_HPP
