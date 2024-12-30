//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_SCRIPT_HPP
#define NOMAD_SCRIPT_HPP

#include "nomad/Nomad.hpp"

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

    [[nodiscard]] NomadId get_id() const { return m_id; }
    [[nodiscard]] const NomadString& get_name() const { return m_name; }
    [[nodiscard]] const NomadString& get_path() const { return m_path; }
    [[nodiscard]] const NomadString& get_source() const { return m_source; }

    void add_parameter(const NomadString& parameter_name, const Type* type);
    [[nodiscard]] NomadId get_parameter_id(const NomadString& parameter_name) const;
    [[nodiscard]] const NomadString& get_parameter_name(NomadId parameter_id) const;
    [[nodiscard]] const Type* get_parameter_type(NomadId parameter_id) const;
    [[nodiscard]] NomadIndex get_parameter_count() const;

    NomadId register_variable(const NomadString& variable_name, const Type* type);
    [[nodiscard]] NomadId get_variable_id(const NomadString& variable_name) const;
    [[nodiscard]] const NomadString& get_variable_name(NomadId variable_id) const;
    void set_variable_type(NomadId variable_id, const Type* type);
    [[nodiscard]] const Type* get_variable_type(NomadId variable_id) const;
    [[nodiscard]] NomadIndex get_variable_count() const;

    void set_return_type(const Type* return_type);
    [[nodiscard]] const Type* get_return_type() const;

    void set_script_start(NomadIndex script_start_index);
    [[nodiscard]] NomadIndex get_script_start() const;

    void set_script_end(NomadIndex script_end_index);
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
