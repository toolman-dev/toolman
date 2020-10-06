// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_LIST_TYPE_H_
#define TOOLMAN_LIST_TYPE_H_

#include <string>
#include <memory>
#include <utility>

#include "src/type.h"

namespace toolman {

class ListType final : public Type {
 public:
    ListType(const std::string &name,
             std::shared_ptr <Type> elem_type,
             unsigned int line_no,
             unsigned int column_no)
            : Type(name, line_no, column_no),
               elem_type_(std::move(elem_type)) {}

    ListType(std::string &&name,
             std::shared_ptr <Type> elem_type,
             unsigned int line_no,
             unsigned int column_no)
            : Type(std::move(name), line_no, column_no),
               elem_type_(std::move(elem_type)) {}

    [[nodiscard]] const std::shared_ptr <Type>& get_elem_type() const
    { return elem_type_; }

    [[nodiscard]] bool is_list() const override { return true; }

 private:
    std::shared_ptr <Type> elem_type_;
};

}  // namespace toolman
#endif  // TOOLMAN_LIST_TYPE_H_
