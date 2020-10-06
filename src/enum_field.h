// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ENUM_FIELD_H_
#define TOOLMAN_ENUM_FIELD_H_

#include <memory>
#include <string>
#include <utility>

#include "src/doc.h"
#include "src/type.h"

namespace toolman {
class EnumField final : public Doc {
 public:
    template <typename S1, typename S2>
    EnumField(S1&& name,
              int value,
              unsigned int line_no,
              unsigned int column_no,
              S2&& file)
              : name_(std::forward<S1>(name)), value_(value),
              Doc(line_no, column_no, std::forward<S2>(file)) {}

    [[nodiscard]] const std::string &get_name() const { return name_; }

    [[nodiscard]] int get_value() const { return value_; }

 private:
    std::string name_;
    int value_;
};
}  // namespace toolman

#endif  // TOOLMAN_ENUM_FIELD_H_
