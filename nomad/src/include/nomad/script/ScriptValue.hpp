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

    void setFloatValue(NomadFloat value);
    void setIntegerValue(NomadInteger value);
    void setIndexValue(NomadIndex value);
    void setBooleanValue(NomadBoolean value);
    void setStringValue(const NomadString& value);
    void setIdValue(NomadId value);
    void setStringValue(const NomadChar* value);

    // Overloaded setter
    void set(NomadFloat value);
    void set(NomadInteger value);
    void set(NomadBoolean value);
    void set(const NomadChar* value);

    // Overloaded getter

    [[nodiscard]] NomadFloat getFloatValue() const;
    [[nodiscard]] NomadInteger getIntegerValue() const;
    [[nodiscard]] NomadIndex get_index_value() const;
    [[nodiscard]] NomadBoolean getBooleanValue() const;
    [[nodiscard]] NomadId getIdValue() const;
    [[nodiscard]] const NomadChar* getStringValue() const;

    void initStringValue();
    void initStringValue(const NomadString& value);

    void move_string_value(ScriptValue& other);

    void freeStringValue() const;

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