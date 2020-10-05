// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_PRIMITIVE_TYPE_H_
#define TOOLMAN_PRIMITIVE_TYPE_H_

#include <string>
#include <utility>

#include "src/type.h"

namespace toolman {
class PrimitiveType final : public Type {
 public:
    // Enumeration of toolman primitive types
    enum class TypeKind : char {
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
}  // namespace toolman
#endif  // TOOLMAN_PRIMITIVE_TYPE_H_
