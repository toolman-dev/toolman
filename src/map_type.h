// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_MAP_TYPE_H_
#define TOOLMAN_MAP_TYPE_H_

#include <memory>
#include <string>
#include <utility>

#include "src/primitive_type.h"
#include "src/type.h"

namespace toolman {

class MapType final : public Type {
 public:
  using KeyType = PrimitiveType;
  using ValueType = Type;

  template <typename S, typename SI>
  MapType(S&& name, std::shared_ptr<PrimitiveType> key_type,
          std::shared_ptr<Type> value_type, SI&& stmt_info)
      : Type(std::forward<S>(name), std::forward<SI>(stmt_info)),
        key_type_(std::move(key_type)),
        value_type_(std::move(value_type)) {}

  [[nodiscard]] bool is_map() const override { return true; }

  [[nodiscard]] const std::shared_ptr<PrimitiveType>& get_key_type() const {
    return key_type_;
  }

  [[nodiscard]] const std::shared_ptr<Type>& get_value_type() const {
    return value_type_;
  }

  [[nodiscard]] std::string to_string() const override {
    return "{" + key_type_->to_string() + ", " + value_type_->to_string() + "}";
  }

 private:
  // In toolman, map key must be primitive type.
  std::shared_ptr<KeyType> key_type_;
  std::shared_ptr<ValueType> value_type_;
};

}  // namespace toolman
#endif  // TOOLMAN_MAP_TYPE_H_
