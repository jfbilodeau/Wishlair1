//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_SCRIPT_HPP
#define NOMAD_SCRIPT_HPP

#include "nomad/nomad.hpp"

#include "nomad/script/FormatString.hpp"
#include "nomad/script/OpCode.hpp"
#include "nomad/script/Variable.hpp"

#include <cstdint>
#include <vector>

namespace nomad {

class Script {
public:
    Script(
        NomadId id,
        NomadString name,
        NomadString path,
        NomadString source
    );

    Script(const Script& other) = delete;

    ~Script();

    [[nodiscard]] NomadId getId() const { return m_id; }
    [[nodiscard]] const NomadString& getName() const { return m_name; }
    [[nodiscard]] const NomadString& getPath() const { return m_path; }
    [[nodiscard]] const NomadString& getSource() const { return m_source; }

    void addParameter(const NomadString& parameterName, const Type* type);
    [[nodiscard]] NomadId getParameterId(const NomadString& parameterName) const;
    [[nodiscard]] const NomadString& getParameterName(NomadId parameterId) const;
    [[nodiscard]] const Type* getParameterType(NomadId parameterId) const;
    [[nodiscard]] NomadIndex getParameterCount() const;

    NomadId registerVariable(const NomadString& variableName, const Type* type);
    [[nodiscard]] NomadId getVariableId(const NomadString& variableName) const;
    [[nodiscard]] const NomadString& getVariableName(NomadId variableId) const;
    void setVariableType(NomadId variableId, const Type* type);
    [[nodiscard]] const Type* getVariableType(NomadId variableId) const;
    [[nodiscard]] NomadIndex getVariableCount() const;

    void setReturnType(const Type* returnType);
    [[nodiscard]] const Type* getReturnType() const;

    void setScriptStart(NomadIndex scriptStartIndex);
    [[nodiscard]] NomadIndex getScriptStart() const;

    void setScriptEnd(NomadIndex scriptEndIndex);
    [[nodiscard]] NomadIndex getScriptEnd() const;

    [[nodiscard]] NomadIndex getScriptLength() const;

//    void set_code(const std::vector<NomadId>& op_codes) { m_op_codes = op_codes; }
//
//    [[nodiscard]] size_t get_code_size() const { return m_op_codes.size(); }
//
//    [[nodiscard]] NomadId get_op_code(NomadIndex index) const { return m_op_codes[index]; }
//
//    [[nodiscard]] NomadId get_id(NomadIndex index) const { return m_op_codes[index]; }
//
//    [[nodiscard]] NomadShort get_short(NomadIndex index) const { return m_op_codes[index]; }
//
//    [[nodiscard]] NomadInteger get_integer(NomadIndex index) const;
//
//    [[nodiscard]] NomadFloat get_float(NomadIndex index) const;

private:
    NomadId m_id;
    NomadString m_name;
    NomadString m_path;
    NomadString m_source;
    NomadIndex m_scriptStartIndex = NOMAD_INVALID_INDEX;
    NomadIndex m_scriptEndIndex = NOMAD_INVALID_INDEX;
    VariableMap m_variables;
    VariableMap m_parameters;
    const Type* m_returnType = nullptr;
};

} // nomad

#endif //NOMAD_SCRIPT_HPP
