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

    void add_parameter(const NomadString& parameter_name, const Type* type);
    [[nodiscard]] NomadId getParameterId(const NomadString& parameter_name) const;
    [[nodiscard]] const NomadString& getParameterName(NomadId parameter_id) const;
    [[nodiscard]] const Type* getParameterType(NomadId parameter_id) const;
    [[nodiscard]] NomadIndex getParameterCount() const;

    NomadId registerVariable(const NomadString& variable_name, const Type* type);
    [[nodiscard]] NomadId getVariableId(const NomadString& variable_name) const;
    [[nodiscard]] const NomadString& get_variable_name(NomadId variable_id) const;
    void set_variable_type(NomadId variable_id, const Type* type);
    [[nodiscard]] const Type* getVariableType(NomadId variable_id) const;
    [[nodiscard]] NomadIndex getVariableCount() const;

    void setReturnType(const Type* return_type);
    [[nodiscard]] const Type* getReturnType() const;

    void setScriptStart(NomadIndex script_start_index);
    [[nodiscard]] NomadIndex getScriptStart() const;

    void setScriptEnd(NomadIndex script_end_index);
    [[nodiscard]] NomadIndex get_script_end() const;

    [[nodiscard]] NomadIndex get_script_length() const;

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
    NomadIndex m_script_start_index = NOMAD_INVALID_INDEX;
    NomadIndex m_script_end_index = NOMAD_INVALID_INDEX;
    VariableMap m_variables;
    VariableMap m_parameters;
    const Type* m_return_type = nullptr;
};

} // nomad

#endif //NOMAD_SCRIPT_HPP
