// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_LIST_TYPE_H_
#define TOOLMAN_LIST_TYPE_H_

#include <memory>
#include <string>
#include <utility>

#include "src/type.h"

namespace toolman {

class ListType final : public Type {
 public:
  template <typename S, typename SI>
  ListType(S&& name, std::shared_ptr<Type> elem_type, SI&& stmt_info)
      : Type(std::forward<S>(name), std::forward<SI>(stmt_info)),
        elem_type_(std::move(elem_type)) {}

  [[nodiscard]] const std::shared_ptr<Type>& get_elem_type() const {
    return elem_type_;
  }

  [[nodiscard]] bool is_list() const override { return true; }

  [[nodiscard]] std::string to_string() const override {
    return "[" + elem_type_->to_string() + "]";
  }

 private:
  std::shared_ptr<Type> elem_type_;
};

}  // namespace toolman
#endif  // TOOLMAN_LIST_TYPE_H_
