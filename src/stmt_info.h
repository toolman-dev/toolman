// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_STMTINFO_H_
#define TOOLMAN_STMTINFO_H_

#include <string>
#include <utility>

namespace toolman {
class StmtInfo final {
 public:
  template <typename S>
  StmtInfo(unsigned int start_line_no, unsigned int start_column_no, S&& file)
      : line_no_({start_line_no, 0}),
        column_no_({start_column_no, 0}),
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

  void set_end_line_no(unsigned int end_line_no) {
    line_no_.second = end_line_no;
  }

  void set_end_column_no(unsigned int end_column_no) {
    column_no_.second = end_column_no;
  }

 protected:
  std::pair<unsigned int, unsigned int> line_no_;
  std::pair<unsigned int, unsigned int> column_no_;
  std::string file_;
};

class HasStmtInfo {
  template <typename SI>
  explicit HasStmtInfo(SI&& stmt_info)
      : stmt_info_(std::forward<SI>(stmt_info)) {}
  [[nodiscard]] const StmtInfo& get_stmt_info() const { return stmt_info_; }
  StmtInfo& mut_stmt_info() { return stmt_info_; }

 protected:
  StmtInfo stmt_info_;
};
}  // namespace toolman
#endif  // TOOLMAN_STMTINFO_H_
