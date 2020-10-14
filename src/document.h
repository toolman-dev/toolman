// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <string>
#include <utility>
#include <vector>

#include "src/custom_type.h"
#include "src/type.h"

namespace toolman {

class Document final {
 public:
  [[nodiscard]] const std::vector<std::shared_ptr<StructType>>&
  get_struct_types() const {
    return struct_types_;
  }
  [[nodiscard]] const std::vector<std::shared_ptr<EnumType>>& get_enum_types()
      const {
    return enum_types_;
  }

  void insert_struct_type(std::shared_ptr<StructType> st) {
    struct_types_.emplace_back(std::move(st));
  }

  void insert_enum_type(std::shared_ptr<EnumType> et) {
    enum_types_.emplace_back(std::move(et));
  }

  [[nodiscard]] std::shared_ptr<std::filesystem::path> get_file() const {
    return file_;
  }

  void set_file(std::shared_ptr<std::filesystem::path> file) {
    file_ = std::move(file);
  }

 private:
  std::vector<std::shared_ptr<StructType>> struct_types_;
  std::vector<std::shared_ptr<EnumType>> enum_types_;
  std::shared_ptr<std::filesystem::path> file_;
};
}  // namespace toolman
#endif  // TOOLMAN_DOC_H_
