// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <string>
#include <utility>

namespace toolman {
class StmtInfo {
 public:
  template <typename S>
  StmtInfo(std::pair<unsigned int, unsigned int> line_no,
           std::pair<unsigned int, unsigned int> column_no, S&& file)
      : line_no_(std::move(line_no)),
        column_no_(std::move(column_no)),
        file_(std::forward<S>(file)) {}

  [[nodiscard]] const std::pair<unsigned int, unsigned int>& get_line_no()
      const {
    return line_no_;
  }
  [[nodiscard]] const std::pair<unsigned int, unsigned int>& get_column_no()
      const {
    return column_no_;
  }
  [[nodiscard]] const std::string& get_file() const { return file_; }

 protected:
  std::pair<unsigned int, unsigned int> line_no_;
  std::pair<unsigned int, unsigned int> column_no_;
  std::string file_;
};

class Doc {
 public:
  template <typename SI>
  explicit Doc(SI&& stmt_info) : stmt_info_(std::forward<SI>(stmt_info)) {}
  [[nodiscard]] const StmtInfo& get_stmt_info() const { return stmt_info_; }

 protected:
  StmtInfo stmt_info_;
};
}  // namespace toolman
#endif  // TOOLMAN_DOC_H_
