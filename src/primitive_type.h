#ifndef PRIMITIVE_TYPE_H
#define PRIMITIVE_TYPE_H

#include <string>
#include <utility>
#include "type.h"

namespace toolman
{

    class PrimitiveType : public Type
    {
    public:
        // Enumeration of toolman primitive types
        enum class TypeKind : char
        {
            Bool,
            I32,
            U32,
            I64,
            U64,
            Float,
            String,
        };
        PrimitiveType(const std::string &name, TypeKind type_kind)
            : Type(name), type_kind_(type_kind) {}
        PrimitiveType(std::string &&name, TypeKind type_kind)
            : Type(std::move(name)), type_kind_(type_kind) {}

        bool is_primitive() const override { return true; }
        bool is_i32() const { return type_kind_ == TypeKind::I32; }
        bool is_u32() const { return type_kind_ == TypeKind::U32; }
        bool is_i64() const { return type_kind_ == TypeKind::I64; }
        bool is_u64() const { return type_kind_ == TypeKind::U64; }
        bool is_float() const { return type_kind_ == TypeKind::Float; }
        bool is_string() const { return type_kind_ == TypeKind::String; }

    private:
        TypeKind type_kind_;
    };
} // namespace toolman
#endif // PRIMITIVE_TYPE_H
