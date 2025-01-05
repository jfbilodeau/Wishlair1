//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_COMPILER_HPP
#define NOMAD_COMPILER_HPP

#include "nomad/nomad.hpp"

#include "nomad/compiler/Operators.hpp"
#include "nomad/compiler/Parser.hpp"
#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/OpCode.hpp"
#include "nomad/script/Runtime.hpp"

#include <unordered_map>
#include <vector>

namespace nomad {

// Forward declarations
class Compiler;
enum class BuildPhase;
class Tokenizer;
class ScriptBuilder;

// Folding function signature
using UnaryFoldingFn = void (*)(const ScriptValue&, ScriptValue&);
using BinaryFoldingFn = void (*)(const ScriptValue&, const ScriptValue&, ScriptValue&);

enum class BuildPhase {
    Init = 1,  // Pre-compile
    Compile,
};

class Compiler {
public:
    explicit Compiler(Runtime* runtime, std::vector<Instruction>& instructions);
    Compiler(const Compiler& other) = delete;
    ~Compiler();

    [[nodiscard]] Runtime* get_runtime() const;

    // Error reporting
    [[noreturn]] void report_error(const NomadString& message);
    [[noreturn]] void report_error(const NomadString& message, Script* script, Tokenizer* tokenizer);
    [[noreturn]] void report_error(const NomadString& message, const NomadString& script_name, NomadIndex line, NomadIndex column);
    [[noreturn]] void report_internal_error(const NomadString& message);

    // Currently compiling script
//    void set_current_script(Script* current_script);
//    [[nodiscard]] Script* get_current_script() const;

    // Statements
    void register_parse_statement_fn(const NomadString& name, ParseStatementFn fn, PreParseStatementFn pre_fn = nullptr);
    [[nodiscard]] bool is_statement(const NomadString& name) const;
    [[nodiscard]] bool get_parse_statement_fn(const NomadString& name, ParseStatementFn& fn) const;
    [[nodiscard]] bool get_pre_parse_statement_fn(const NomadString& name, PreParseStatementFn& fn) const;
    void get_registered_statements(std::vector<NomadString>& parsers) const;

    // Constant folding
    void register_unary_operator(UnaryOperator op, const Type* operand, const Type* result, const NomadString& op_code_name, UnaryFoldingFn fn);
    void register_binary_operator(BinaryOperator op, const Type* lhs, const Type* rhs, const Type* result, const NomadString& op_code_name, BinaryFoldingFn fn);
    [[nodiscard]] const Type* get_unary_operator_result_type(UnaryOperator op, const Type* operand_type) const;
    [[nodiscard]] const Type* get_binary_operator_result_type(BinaryOperator op, const Type* lhs_type, const Type* rhs_type) const;
    [[nodiscard]] NomadId get_unary_operator_op_code_id(UnaryOperator op, const Type* operand) const;
    [[nodiscard]] NomadId get_binary_operator_op_code_id(BinaryOperator op, const Type* lhs, const Type* rhs) const;
    [[nodiscard]] bool fold_unary(
        UnaryOperator op,
        const Type* operand_type,
        const ScriptValue& value,
        ScriptValue& result
    ) const;
    [[nodiscard]] bool fold_binary(
        BinaryOperator op,
        const Type* lhs_type,
        const ScriptValue& lhs,
        const Type* rhs_type,
        const ScriptValue& rhs,
        ScriptValue& result
    ) const;

    // Identifier identification
    IdentifierType get_identifier_type(const NomadString& name, Script* script);
    void get_identifier_definition(const NomadString& name, Script* script, IdentifierDefinition& definition);

    // Opcode generation
    [[nodiscard]] NomadIndex get_op_code_size() const;

    NomadIndex add_op_code(NomadId op_code);
    NomadIndex add_op_code(const NomadString& op_code_name);
    NomadIndex add_id(NomadId id);
    NomadIndex add_index(NomadIndex index);
    NomadIndex add_integer(NomadInteger value);
    NomadIndex add_float(NomadFloat value);
    NomadIndex add_load_value(const Type* type, const ScriptValue& value);
    NomadIndex add_load_boolean_value(bool value);
    NomadIndex add_load_float_value(NomadFloat value);
    NomadIndex add_load_integer_value(NomadInteger value);
    NomadIndex add_load_string_value(const NomadString& value);

    NomadIndex add_push_result(const Type* type);
    NomadIndex add_pop_intermediate(const Type* type);

    NomadIndex add_script_call(NomadId target_script_id);
    NomadIndex add_command_call(NomadId command_id);

    void set_op_code(NomadIndex index, const NomadString& op_code_name);
    void set_op_code(NomadIndex index, NomadId op_code_id);
    void set_id(NomadIndex index, NomadId id);
    void set_index(NomadIndex index, NomadIndex value);

    NomadId register_script_file(const NomadString& script_name, const NomadString& file_name, const NomadString& source);
    NomadId register_script_source(const NomadString& script_name, const NomadString& file_name, const NomadString& source);
    void set_script_node(NomadId script_id, std::unique_ptr<ScriptNode> ast);
    void load_scripts_from_path(const NomadString& path);
    void compile_scripts();
    void add_script_link(NomadId script_id, NomadIndex call_index);
    void link_scripts();

    // Generate script names for internal scripts
    NomadString generate_script_name(const NomadString& generated_name, const NomadString& host_script_name, NomadIndex line);
    NomadString generate_script_name(const NomadString& name, const Script* script, NomadIndex line);

private:
    struct ScriptSource {
        NomadString source;
        std::unique_ptr<ScriptNode> ast;
        Script* script;
    };

    struct ScriptFile {
        NomadString script_name;
        NomadString file_name;
        NomadString source;
        Tokenizer tokens;

        NomadId script_id;
    };

//    void compile_init_script(Script* script);
    void pre_parse_script(ScriptFile& script_file);
    void parse_script(ScriptFile& file);
    void compile_script(ScriptSource& source);
    void scan_directory_for_scripts(const NomadString& base_path, const NomadString& sub_path, NomadIndex max_depth);

    Runtime* m_runtime;

    struct ParseStatementFnRegistration {
        PreParseStatementFn pre_fn;
        ParseStatementFn fn;
    };

    struct OpCodeRegistration {
        NomadId id;
        NomadString name;
        InstructionFn fn;
    };

    struct UnaryOperatorRegistration {
        UnaryOperator op;
        const Type* operand;
        const Type* result;
        NomadId op_code_id;
        UnaryFoldingFn fn;
    };

    struct BinaryOperatorRegistration {
        BinaryOperator op;
        const Type* lhs;
        const Type* rhs;
        const Type* result;
        NomadId op_code_id;
        BinaryFoldingFn fn;
    };

    struct ScriptLink {
        NomadId script_id;
        NomadIndex call_index;
    };

//    Script* m_current_script;
    std::unordered_map<NomadString, ParseStatementFnRegistration> m_statements;
    std::vector<OpCodeRegistration> m_op_code_registrations;
    std::vector<UnaryOperatorRegistration> m_unary_operators;
    std::vector<BinaryOperatorRegistration> m_binary_operators;
    std::vector<ScriptFile> m_files;
    std::vector<ScriptSource> m_sources;
    std::vector<ScriptLink> m_script_links;
    std::vector<Instruction>& m_instructions;
};

} // nomad

#endif //NOMAD_COMPILER_HPP
