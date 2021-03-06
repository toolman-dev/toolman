// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_STMTINFO_H_
#define TOOLMAN_STMTINFO_H_

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

namespace toolman {
class StmtInfo final {
 public:
  StmtInfo(unsigned int start_line_no, unsigned int start_column_no,
           std::shared_ptr<std::filesystem::path> source)
      : line_no_({start_line_no, 0}),
        column_no_({start_column_no, 0}),
        source_(std::move(source)) {}
  StmtInfo(std::pair<unsigned int, unsigned int> line_no,
           std::pair<unsigned int, unsigned int> column_no,
           std::shared_ptr<std::filesystem::path> source)
      : line_no_(std::move(line_no)),
        column_no_(std::move(column_no)),
        source_(std::move(source)) {}
  [[nodiscard]] const std::pair<unsigned int, unsigned int>& get_line_no()
      const {
    return line_no_;
  }
  [[nodiscard]] const std::pair<unsigned int, unsigned int>& get_column_no()
      const {
    return column_no_;
  }
  [[nodiscard]] std::shared_ptr<std::filesystem::path> get_source() const {
    return source_;
  }

  void set_end_line_no(unsigned int end_line_no) {
    line_no_.second = end_line_no;
  }

  void set_end_column_no(unsigned int end_column_no) {
    column_no_.second = end_column_no;
  }

 protected:
  std::pair<unsigned int, unsigned int> line_no_;
  std::pair<unsigned int, unsigned int> column_no_;
  std::shared_ptr<std::filesystem::path> source_;
};

class HasStmtInfo {
 public:
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
