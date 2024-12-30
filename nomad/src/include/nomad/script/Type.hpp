//
// Created by jbilodeau on 2024-09-17.
//

#ifndef NOMAD_TYPE_HPP
#define NOMAD_TYPE_HPP

#include "nomad/script/ScriptValue.hpp"

#include "nomad/script/OpCode.hpp"

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

class ScriptType;

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]] virtual NomadString get_name() const = 0;
    virtual void init_value(ScriptValue& value) const;
    virtual void free_value(ScriptValue& value) const;
    virtual void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const = 0;

    // Convert value to string.
    virtual void to_string(const ScriptValue& value, NomadString& string) const = 0;
    [[nodiscard]] NomadString to_string(const ScriptValue& value) const;

    // Used by compiler to determine if stack value needs to be initialized and/or freed.
    [[nodiscard]] bool needs_init() const;
    [[nodiscard]] bool needs_free() const;
    [[nodiscard]] bool need_copy() const;

    // Can the type hold a value or is it void?
    [[nodiscard]] virtual bool is_void() const;

    // Determine if two types are the same
    [[nodiscard]] virtual bool same_type(const Type* other) const;

    // Return the type as a callback type or `nullptr` if it is not a callback
    [[nodiscard]] virtual const ScriptType* as_callback() const;

protected:
    [[nodiscard]] virtual NomadId get_init_op_code() const;
    [[nodiscard]] virtual NomadId get_free_op_code() const;
    [[nodiscard]] virtual NomadId get_copy_op_code() const;
};

const NomadString VOID_TYPE_NAME = "void";
class VoidType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;

    [[nodiscard]] bool is_void() const override;
};

const NomadString ID_TYPE_NAME = "id";
class IdType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
};

const NomadString INTEGER_TYPE_NAME = "integer";
class IntegerType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
};

const NomadString FLOAT_TYPE_NAME = "float";
class FloatType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
};

const NomadString BOOLEAN_TYPE_NAME = "boolean";
const NomadString BOOLEAN_TRUE_STRING = "true";
const NomadString BOOLEAN_FALSE_STRING = "false";
class BooleanType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
};

const NomadString STRING_TYPE_NAME = "string";
class StringType final : public Type {
public:
    [[nodiscard]] NomadString get_name() const override;
    void init_value(ScriptValue& value) const override;
    void free_value(ScriptValue& value) const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
};

const NomadString SCRIPT_TYPE_NAME = "callback";
class ScriptType final : public Type {
public:
    ScriptType(const std::vector<const Type*>& parameter_types, const Type* return_type);

    [[nodiscard]] NomadString get_name() const override;
    void copy_value(const ScriptValue& source_value, ScriptValue& destination_value) const override;
    void to_string(const ScriptValue& value, NomadString& string) const override;
    [[nodiscard]] bool same_type(const Type* other) const override;

    [[nodiscard]] NomadIndex get_parameter_count() const;
    [[nodiscard]] const Type* get_parameter_type(NomadIndex index) const;
    [[nodiscard]] const Type* get_return_type() const;

    [[nodiscard]] const ScriptType* as_callback() const override;

private:
    NomadString m_name;
    const Type* m_return_type;
    const std::vector<const Type*> m_parameter_types;
};


} // nomad

#endif // NOMAD_TYPE_HPP
