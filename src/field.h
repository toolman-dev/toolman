// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_FIELD_H_
#define TOOLMAN_FIELD_H_

#include <memory>
#include <string>
#include <utility>

#include "src/doc.h"
#include "src/type.h"

namespace toolman {

class Field final : public Doc {
 public:
  template <typename S, typename SI>
  Field(S&& name, std::shared_ptr<Type> type, bool optional, SI&& stmt_info)
      : type_(std::forward<S>(type)),
        name_(name),
        optional_(optional),
        Doc(std::forward<SI>(stmt_info)) {}

  [[nodiscard]] const std::string& get_name() const { return name_; }

  [[nodiscard]] bool is_optional() const { return optional_; }

 private:
  std::shared_ptr<Type> type_;
  std::string name_;
  bool optional_;
};

}  // namespace toolman

#endif  // TOOLMAN_FIELD_H_
