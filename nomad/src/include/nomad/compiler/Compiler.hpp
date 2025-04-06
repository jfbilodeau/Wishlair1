//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include "nomad/nomad.hpp"

#include "nomad/compiler/Operators.hpp"
#include "nomad/compiler/Parser.hpp"
#include "nomad/compiler/Tokenizer.hpp"

#include "nomad/script/Command.hpp"
#include "nomad/script/OpCode.hpp"

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

    [[nodiscard]] Runtime* getRuntime() const;

    // Error reporting
    [[noreturn]] void reportError(const NomadString& message);
    [[noreturn]] void reportError(const NomadString& message, Script* script, Tokenizer* tokenizer);
    [[noreturn]] void reportError(const NomadString& message, const NomadString& scriptName, NomadIndex line, NomadIndex column);
    [[noreturn]] void reportInternalError(const NomadString& message);

    // Currently compiling script
//    void set_current_script(Script* current_script);
//    [[nodiscard]] Script* get_current_script() const;

    // Statements
    void registerParseStatementFn(const NomadString& name, ParseStatementFn fn, PreParseStatementFn preFn = nullptr);
    [[nodiscard]] bool isStatement(const NomadString& name) const;
    [[nodiscard]] bool getParseStatementFn(const NomadString& name, ParseStatementFn& fn) const;
    [[nodiscard]] bool getGetPreParseStatementFn(const NomadString& name, PreParseStatementFn& fn) const;
    void getRegisteredStatements(std::vector<NomadString>& parsers) const;

    // Constant folding
    void registerUnaryOperator(UnaryOperator op, const Type* operand, const Type* result, const NomadString& opCodeName, UnaryFoldingFn fn);
    void registerBinaryOperator(BinaryOperator op, const Type* lhs, const Type* rhs, const Type* result, const NomadString& opCodeName, BinaryFoldingFn fn);
    [[nodiscard]] const Type* getUnaryOperatorResultType(UnaryOperator op, const Type* operandType) const;
    [[nodiscard]] const Type* getBinaryOperatorResultType(BinaryOperator op, const Type* lhsType, const Type* rhsType) const;
    [[nodiscard]] NomadId getUnaryOperatorOpCodeId(UnaryOperator op, const Type* operand) const;
    [[nodiscard]] NomadId getBinaryOperatorOpCodeId(BinaryOperator op, const Type* lhs, const Type* rhs) const;
    [[nodiscard]] bool foldUnary(
        UnaryOperator op,
        const Type* operandType,
        const ScriptValue& value,
        ScriptValue& result
    ) const;
    [[nodiscard]] bool foldBinary(
        BinaryOperator op,
        const Type* lhsType,
        const ScriptValue& lhs,
        const Type* rhsType,
        const ScriptValue& rhs,
        ScriptValue& result
    ) const;

    // Identifier identification
    IdentifierType getIdentifierType(const NomadString& name, Script* script);
    void getIdentifierDefinition(const NomadString& name, Script* script, IdentifierDefinition& definition);

    // Opcode generation
    [[nodiscard]] NomadIndex getOpCodeSize() const;

    NomadIndex addOpCode(NomadId opCode);
    NomadIndex addOpCode(const NomadString& opCodeName);
    NomadIndex addId(NomadId id);
    NomadIndex addIndex(NomadIndex index);
    NomadIndex addInteger(NomadInteger value);
    NomadIndex addFloat(NomadFloat value);
    NomadIndex addLoadValue(const Type* type, const ScriptValue& value);
    NomadIndex addLoadBooleanValue(bool value);
    NomadIndex addLoadFloatValue(NomadFloat value);
    NomadIndex addLoadIntegerValue(NomadInteger value);
    NomadIndex addLoadStringValue(const NomadString& value);

    NomadIndex addPushResult(const Type* type);
    NomadIndex addPopIntermediate(const Type* type);

    NomadIndex addScriptCall(NomadId targetScriptId);
    NomadIndex addCommandCall(NomadId commandId);

    void setOpCode(NomadIndex index, const NomadString& opCodeName);
    void setOpCode(NomadIndex index, NomadId opCodeId);
    void setId(NomadIndex index, NomadId id);
    void setIndex(NomadIndex index, NomadIndex value);

    NomadId registerScriptFile(const NomadString& scriptName, const NomadString& fileName, const NomadString& source);
    NomadId registerScriptSource(const NomadString& scriptName, const NomadString& fileName, const NomadString& source);
    void setScriptNode(NomadId scriptId, std::unique_ptr<ScriptNode> ast);
    void loadScriptsFromPath(const NomadString& path);
    void compileScripts();
    void addScriptLink(NomadId scriptId, NomadIndex callIndex);
    void linkScripts();

    // Generate script names for internal scripts
    NomadString generateScriptName(const NomadString& generatedName, const NomadString& hostScriptName, NomadIndex line);
    NomadString generateScriptName(const NomadString& name, const Script* script, NomadIndex line);

private:
    struct ScriptSource {
        NomadString source;
        std::unique_ptr<ScriptNode> ast;
        Script* script;
    };

    struct ScriptFile {
        NomadString scriptName;
        NomadString fileName;
        NomadString source;
        Tokenizer tokens;

        NomadId scriptId;
    };

//    void compile_init_script(Script* script);
    void preParseScript(ScriptFile& scriptFile);
    void parseScript(ScriptFile& file);
    void compileScript(ScriptSource& source);
    void scanDirectoryForScripts(const NomadString& basePath, const NomadString& sub_path, NomadIndex max_depth);

    Runtime* m_runtime;

    struct ParseStatementFnRegistration {
        PreParseStatementFn preFn;
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
        NomadId opCodeId;
        UnaryFoldingFn fn;
    };

    struct BinaryOperatorRegistration {
        BinaryOperator op;
        const Type* lhs;
        const Type* rhs;
        const Type* result;
        NomadId opCodeId;
        BinaryFoldingFn fn;
    };

    struct ScriptLink {
        NomadId scriptId;
        NomadIndex callIndex;
    };

//    Script* m_current_script;
    std::unordered_map<NomadString, ParseStatementFnRegistration> m_statements;
    std::vector<OpCodeRegistration> m_opCodeRegistrations;
    std::vector<UnaryOperatorRegistration> m_unaryOperators;
    std::vector<BinaryOperatorRegistration> m_binaryOperators;
    std::vector<ScriptFile> m_files;
    std::vector<ScriptSource> m_sources;
    std::vector<ScriptLink> m_scriptLinks;
    std::vector<Instruction>& m_instructions;
};

} // nomad

