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
    [[nodiscard]] virtual const std::string &get_name() const { return name_; }

    [[nodiscard]] virtual bool is_primitive() const { return false; }

    [[nodiscard]] virtual bool is_enum() const { return false; }

    [[nodiscard]] virtual bool is_struct() const { return false; }

    [[nodiscard]] virtual bool is_list() const { return false; }

    [[nodiscard]] virtual bool is_map() const { return false; }

    [[nodiscard]] virtual bool is_oneof() const { return false; }

    [[nodiscard]] virtual std::string to_string() const = 0;

 protected:
    template <typename S1, typename S2>
    Type(S1&& name,
         unsigned int line_no,
         unsigned int column_no,
         S2&& file)
         : name_(std::forward<S1>(name)), Doc(line_no, column_no,
            std::forward<S2>(file)) {}

    std::string name_;
};

}  // namespace toolman
#endif  // TOOLMAN_TYPE_H_
