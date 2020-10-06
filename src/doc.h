// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <utility>
#include <string>

namespace toolman {
class Doc {
 public:
    Doc(unsigned int line_no, unsigned int column_no, std::string file)
        : line_no_(line_no), column_no_(column_no), file_(std::move(file)) {}
    [[nodiscard]] unsigned int get_line_no() const { return line_no_; }
    [[nodiscard]] unsigned int get_column_no() const { return column_no_; }
    [[nodiscard]] std::string get_file() const { return file_; }
 protected:
    unsigned int line_no_;
    unsigned int column_no_;
    std::string file_;
};
}  // namespace toolman

#endif //TOOLMAN_DOC_H_
