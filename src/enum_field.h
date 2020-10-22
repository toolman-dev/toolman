// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ENUM_FIELD_H_
#define TOOLMAN_ENUM_FIELD_H_

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "src/stmt_info.h"
#include "src/type.h"

namespace toolman {
class EnumField final : public HasStmtInfo {
 public:
  template <typename S, typename SI>
  EnumField(S&& name, SI&& stmt_info)
      : name_(std::forward<S>(name)),
        HasStmtInfo(std::forward<SI>(stmt_info)) {}

  [[nodiscard]] const std::string& get_name() const { return name_; }

  [[nodiscard]] int get_value() const { return value_; }

  // set value to enum field, return false when value already exists.
  bool set_value(int value) {
    value_ = value;
    return value_mapping_.emplace(value, *this).second;
  }

  static std::optional<EnumField> get_by_value(int value) {
    if (auto got = value_mapping_.find(value); got != value_mapping_.end()) {
      return std::make_optional(got->second);
    } else {
      return std::nullopt;
    }
  }

 private:
  std::string name_;
  int value_;
  inline static std::unordered_map<int, EnumField> value_mapping_;
};
}  // namespace toolman

#endif  // TOOLMAN_ENUM_FIELD_H_
