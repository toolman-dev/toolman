// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ENUM_FIELD_H_
#define TOOLMAN_ENUM_FIELD_H_

#include <memory>
#include <string>
#include <utility>

#include "src/type.h"

namespace toolman {
class EnumField final {
 public:
    EnumField(const std::string &name, int value)
            : name_(name), value_(value) {}

    EnumField(std::string &&name, int value)
            : name_(std::move(name)), value_(value) {}

    [[nodiscard]] const std::string &get_name() const { return name_; }

    [[nodiscard]] int get_value() const { return value_; }

 private:
    std::string name_;
    int value_;
};
}  // namespace toolman

#endif  //TOOLMAN_ENUM_FIELD_H_
