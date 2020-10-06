// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_TYPE_ERROR_H_
#define TOOLMAN_TYPE_ERROR_H_

#include <stdexcept>
#include <string>
#include <utility>

namespace toolman {
class TypeError : public std::runtime_error {
 public:
    template <typename S>
    TypeError(S&& message,
              unsigned int line_no,
              unsigned int column_no)
              : message_(std::forward<S>(message)), line_no_(line_no),
                  column_no_(column_no);
 private:
    std::string message_;
    unsigned int line_no_;
    unsigned int column_no_;
};

}  // namespace toolman
#endif  // TOOLMAN_TYPE_ERROR_H_
