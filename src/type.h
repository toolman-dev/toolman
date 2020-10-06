// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPE_H_
#define TOOLMAN_TYPE_H_

#include <string>
#include <utility>

#include "src/doc.h"

namespace toolman {

class Type : public Doc {
 public:
  [[nodiscard]] virtual const std::string& get_name() const { return name_; }

  [[nodiscard]] virtual bool is_primitive() const { return false; }

  [[nodiscard]] virtual bool is_enum() const { return false; }

  [[nodiscard]] virtual bool is_struct() const { return false; }

  [[nodiscard]] virtual bool is_list() const { return false; }

  [[nodiscard]] virtual bool is_map() const { return false; }

  [[nodiscard]] virtual bool is_oneof() const { return false; }

  [[nodiscard]] virtual std::string to_string() const = 0;

 protected:
  template <typename S, typename SI>
  Type(S&& name, SI&& stmt_info)
      : name_(std::forward<S>(name)), Doc(std::forward<S>(stmt_info)) {}

  std::string name_;
};

}  // namespace toolman
#endif  // TOOLMAN_TYPE_H_
