// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_FIELD_H_
#define TOOLMAN_FIELD_H_

#include <memory>
#include <string>
#include <utility>

#include "src/type.h"
#include "src/doc.h"

namespace toolman {

class Field final : public Doc {
 public:
    Field(std::shared_ptr<Type> type,
          const std::string& name,
          bool optional,
          unsigned int line_no,
          unsigned int column_no)
    : type_(std::move(type)), name_(name),
        optional_(optional), Doc(line_no, column_no) {}

    Field(std::shared_ptr<Type> type,
          std::string&& name,
          bool optional,
          unsigned int line_no,
          unsigned int column_no)
    : type_(std::move(type)), name_(std::move(name)),
        optional_(optional), Doc(line_no, column_no) {}

    [[nodiscard]] const std::string& get_name() const { return name_; }

    [[nodiscard]] bool is_optional() const { return optional_; }

 private:
    std::shared_ptr<Type> type_;
    std::string name_;
    bool optional_;
};

}  // namespace toolman

#endif  // TOOLMAN_FIELD_H_
