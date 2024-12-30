//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_SCRIPTBUILDER_HPP
#define NOMAD_SCRIPTBUILDER_HPP

#include "nomad/script/FormatString.hpp"
#include "nomad/script/Runtime.hpp"
#include "nomad/script/Script.hpp"

#include "nomad/compiler/CompilerException.hpp"
#include "nomad/compiler/Identifier.hpp"

namespace nomad {

// Forward declarations
class Compiler;
class Tokenizer;

class ScriptBuilderException : public CompilerException {
public:
    explicit ScriptBuilderException(const NomadString& message) : CompilerException(message) {}
};

class ScriptBuilder {
public:
    ScriptBuilder(Script* script, Compiler* compiler);
    ScriptBuilder(ScriptBuilder&) = delete;
    ~ScriptBuilder();

    [[nodiscard]] Script* get_script() const;
    [[nodiscard]] Compiler* get_compiler() const;
    [[nodiscard]] Runtime* get_runtime() const;

//    void add_parameter(const NomadString& parameter_name, Type* type);
//    [[nodiscard]] NomadId get_parameter_id(const NomadString& parameter_name) const;
//    [[nodiscard]] const NomadString& get_parameter_name(NomadId parameter_id) const;
//    [[nodiscard]] const Type* get_parameter_type(NomadId parameter_id) const;
//    [[nodiscard]] NomadIndex get_parameter_count() const;

    [[noreturn]] void report_error(const NomadString& message, Tokenizer* parser);
    [[noreturn]] void report_error(const NomadString& message, NomadIndex line, NomadIndex column);

    IdentifierType get_identifier_type(const NomadString& name);
    void get_identifier_definition(const NomadString& name, IdentifierDefinition& definition);

    NomadId register_script_variable(const NomadString& name, Type* type);
    [[nodiscard]] NomadId get_script_variable_id(const NomadString& name);

    NomadId register_format_string(const FormatString& format_string_data);
    [[nodiscard]] NomadId get_format_string_id(const NomadString& format_string) const;

    [[nodiscard]] NomadIndex get_op_code_size() const;

    NomadIndex add_op_code(NomadId op_code);
    NomadIndex add_op_code(const NomadString& op_code_name);
    NomadIndex add_id(NomadId id);
    NomadIndex add_index(NomadIndex index);
    NomadIndex add_short(NomadShort value);
    NomadIndex add_integer(NomadInteger value);
    NomadIndex add_float(NomadFloat value);
    NomadIndex add_load_value(const Type* type, const ScriptValue& value);
    NomadIndex add_load_boolean_value(bool value);
    NomadIndex add_load_float_value(NomadFloat value);
    NomadIndex add_load_integer_value(NomadInteger value);
    NomadIndex add_load_string_value(const NomadString& value);

    NomadIndex add_push_result(const Type* type);
    NomadIndex add_pop_intermediate(const Type* type);

    NomadIndex add_script_call(NomadId script_id);

    void set_op_code(NomadIndex index, const NomadString& op_code_name);
    void set_op_code(NomadIndex index, NomadId op_code_id);
    void set_id(NomadIndex index, NomadId id);
    void set_short(NomadIndex index, NomadShort value);

    void insert_instruction(NomadIndex index, const NomadString& instruction_name);
    void insert_id(NomadIndex index, NomadId id);
    void insert_short(NomadIndex index, NomadShort value);

    [[nodiscard]] NomadId get_op_code_id(NomadIndex index) const;

    void end_script();

private:
    void report_internal_error(const NomadString& message);

    Script* m_script;
    Compiler* m_compiler;
    VariableMap m_parameters;
    std::vector<NomadId> m_op_codes;
};

} // nomad

#endif // NOMAD_SCRIPTBUILDER_HPP
