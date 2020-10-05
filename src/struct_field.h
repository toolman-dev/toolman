// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_STRUCT_FIELD_H_
#define TOOLMAN_STRUCT_FIELD_H_

#include <memory>
#include <string>
#include <utility>

#include "src/type.h"

namespace toolman {

class StructField {
 public:
    StructField(std::shared_ptr<Type> type,
    const std::string& name,
    bool optional)
    : type_(type), name_(name), optional_(optional) {}

    StructField(std::shared_ptr<Type> type,
    const std::string&& name,
    bool optional)
    : type_(type), name_(std::move(name)), optional_(optional) {}

    const std::string& get_name() const { return name_; }

    bool is_optional() const { return optional_; }

 private:
    std::shared_ptr<Type> type_;
    std::string name_;
    bool optional_;
};

}  // namespace toolman

#endif  // TOOLMAN_STRUCT_FIELD_H_
