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

  template <typename S, typename SI>
  PrimitiveType(S&& name, TypeKind type_kind, SI&& stmt_info)
      : Type(std::forward<S>(name), std::forward<SI>(stmt_info)),
        type_kind_(type_kind) {}

  [[nodiscard]] bool is_primitive() const override { return true; }

  [[nodiscard]] bool is_bool() const { return type_kind_ == TypeKind::Bool; }

  [[nodiscard]] bool is_i32() const { return type_kind_ == TypeKind::I32; }

  [[nodiscard]] bool is_u32() const { return type_kind_ == TypeKind::U32; }

  [[nodiscard]] bool is_i64() const { return type_kind_ == TypeKind::I64; }

  [[nodiscard]] bool is_u64() const { return type_kind_ == TypeKind::U64; }

  [[nodiscard]] bool is_float() const { return type_kind_ == TypeKind::Float; }

  [[nodiscard]] bool is_string() const {
    return type_kind_ == TypeKind::String;
  }

  bool operator==(const Type& rhs) const override {
    if (!rhs.is_primitive()) {
      return false;
    }
    return operator==(dynamic_cast<const PrimitiveType&>(rhs));
  }

  bool operator==(const PrimitiveType& rhs) const {
    return type_kind_ == rhs.type_kind_;
  }

  [[nodiscard]] std::string to_string() const override {
    switch (type_kind_) {
      case TypeKind::Bool:
        return "bool";
      case TypeKind::I32:
        return "i32";
      case TypeKind::U32:
        return "u32";
      case TypeKind::I64:
        return "i64";
      case TypeKind::U64:
        return "u64";
      case TypeKind::Float:
        return "float";
      case TypeKind::String:
        return "string";
    }
  }

 private:
  TypeKind type_kind_;
};
}  // namespace toolman
#endif  // TOOLMAN_PRIMITIVE_TYPE_H_
