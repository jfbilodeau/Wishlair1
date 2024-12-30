//
// Created by jbilodeau on 2024-09-17.
//

#ifndef NOMAD_SCRIPT_VALUE_HPP
#define NOMAD_SCRIPT_VALUE_HPP

#include "nomad/nomad.hpp"

namespace nomad {

const NomadString SCRIPT_INTERNAL_NAME_PREFIX = "$";

class ScriptValue {
public:
    ScriptValue();
    ScriptValue(const ScriptValue& other);
    explicit ScriptValue(NomadFloat value);
    explicit ScriptValue(NomadInteger value);
    explicit ScriptValue(NomadIndex value);
    explicit ScriptValue(NomadBoolean value);
    explicit ScriptValue(NomadId value);
    explicit ScriptValue(const NomadString& value);

    void set_float_value(NomadFloat value);
    void set_integer_value(NomadInteger value);
    void set_index_value(NomadIndex value);
    void set_boolean_value(NomadBoolean value);
    void set_string_value(const NomadString& value);
    void set_id_value(NomadId value);
    void set_string_value(const NomadChar* value);

    // Overloaded setter
    void set(NomadFloat value);
    void set(NomadInteger value);
    void set(NomadBoolean value);
    void set(const NomadChar* value);

    // Overloaded getter

    [[nodiscard]] NomadFloat get_float_value() const;
    [[nodiscard]] NomadInteger get_integer_value() const;
    [[nodiscard]] NomadIndex get_index_value() const;
    [[nodiscard]] NomadBoolean get_boolean_value() const;
    [[nodiscard]] NomadId get_id_value() const;
    [[nodiscard]] const NomadChar* get_string_value() const;

    void init_string_value();
    void init_string_value(const NomadString& value);

    void move_string_value(ScriptValue& other);

    void free_string_value() const;

private:
    union {
        NomadInteger m_integer_value = 0;
        NomadFloat m_float_value;
        NomadIndex m_index_value;
        NomadBoolean m_boolean_value;
        NomadId m_id_value;
        NomadChar* m_string_value;
    };
};

} // namespace nomad

#endif // NOMAD_SCRIPT_VALUE_HPP