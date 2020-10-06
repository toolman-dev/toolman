// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_CUSTOM_TYPE_H_
#define TOOLMAN_CUSTOM_TYPE_H_

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "src/type.h"

namespace toolman {

class Field;
class EnumField;

template <typename F>
class CustomType : public Type {
 public:
  template <typename S, typename SI>
  CustomType(S&& name, SI&& stmt_info, bool is_public)
      : Type(std::forward<S>(name), std::forward<S>(stmt_info)),
        is_public_(is_public) {}

  bool append_field(F f) {
    // returns false when there is a conflict of field names
    if (get_field_by_name(f.get_name()).has_value()) {
      return false;
    }
    fields_.push_back(std::move(f));
    return true;
  }

  [[nodiscard]] std::optional<F> get_field_by_name(
      const std::string& field_name) const {
    for (const auto& f : fields_) {
      if (f.get_name() == field_name) {
        return std::make_optional(f);
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] bool is_public() const {return is_public_;}
 private:
  std::vector<F> fields_;
  bool is_public_;
};

class StructType final : public CustomType<Field> {
 public:
  using CustomType::CustomType;
  [[nodiscard]] bool is_struct() const override { return true; }
};

class EnumType final : public CustomType<EnumField> {
 public:
  using CustomType::CustomType;
  [[nodiscard]] bool is_enum() const override { return true; }
};

class OneofType final : public CustomType<Field> {
 public:
  using CustomType::CustomType;
  [[nodiscard]] bool is_oneof() const override { return true; }
};

}  // namespace toolman

#endif  // TOOLMAN_CUSTOM_TYPE_H_
