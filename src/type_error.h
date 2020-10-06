// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPE_ERROR_H_
#define TOOLMAN_TYPE_ERROR_H_

#include <stdexcept>
#include <string>

namespace toolman {
class TypeError : public std::runtime_error {
 public:
    TypeError(std::string message_,
              unsigned int line_no_,
              unsigned int column_no_);
 private:
    std::string message_;
    unsigned int line_no_;
    unsigned int column_no_;
};

}  // namespace toolman
#endif  // TOOLMAN_TYPE_ERROR_H_
