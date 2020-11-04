// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <string>
#include <utility>
#include <vector>

#include "src/api.h"
#include "src/custom_type.h"
#include "src/option.h"
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

  [[nodiscard]] const std::vector<std::shared_ptr<Option>>& get_options()
      const {
    return options_;
  }

  void insert_struct_type(std::shared_ptr<StructType> st) {
    struct_types_.emplace_back(std::move(st));
  }

  void insert_enum_type(std::shared_ptr<EnumType> et) {
    enum_types_.emplace_back(std::move(et));
  }

  void insert_option(std::shared_ptr<Option> option) {
    options_.emplace_back(std::move(option));
  }

  [[nodiscard]] std::shared_ptr<std::filesystem::path> get_source() const {
    return source_;
  }

  void set_source(std::shared_ptr<std::filesystem::path> source) {
    source_ = std::move(source);
  }

  void insert_api_group(ApiGroup api_group) {
    api_groups_.emplace_back(std::move(api_group));
  }

 private:
  std::vector<std::shared_ptr<StructType>> struct_types_;
  std::vector<std::shared_ptr<EnumType>> enum_types_;
  std::vector<std::shared_ptr<Option>> options_;
  std::vector<ApiGroup> api_groups_;
  std::shared_ptr<std::filesystem::path> source_;
};
}  // namespace toolman
#endif  // TOOLMAN_DOC_H_
