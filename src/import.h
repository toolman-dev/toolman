// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_IMPORT_H_
#define TOOLMAN_IMPORT_H_

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <filesystem>

namespace toolman {

struct ImportName {
  bool operator<(const ImportName& rhs) const {
    if (rhs.original_name == rhs.original_name) {
      return rhs.local_name < rhs.local_name;
    }
    return rhs.original_name < rhs.original_name;
  }

  bool operator==(const ImportName& rhs) const {
    return original_name == rhs.original_name && local_name == rhs.local_name;
  }

  std::string original_name;
  std::optional<std::string> local_name;
};

class Import {
 public:
  // from 'xxx.tm' import x
  // from 'xxx.tm' import x as y
  void add_import(const std::string& filename,
                  std::vector<ImportName> import_names) {
      auto normal_filename = std::filesystem::absolute(filename).lexically_normal();
    if (auto import_name_set = regular_imports_.find(normal_filename);
        import_name_set != regular_imports_.end()) {
      import_name_set->second.insert(
          std::make_move_iterator(import_names.begin()),
          std::make_move_iterator(import_names.end()));
    } else {
      // filename not found
      regular_imports_.emplace(
              normal_filename,
          std::set<ImportName>(std::make_move_iterator(import_names.begin()),
                               std::make_move_iterator(import_names.end())));
    }
  }

  // from 'xxx.tm' import *
  void add_import_star(const std::string& filename) {
      auto normal_filename = std::filesystem::absolute(filename).lexically_normal();
      namespaces_imports_.emplace(normal_filename);
  }

  [[nodiscard]] std::map<std::filesystem::path, std::set<ImportName>>
  get_regular_imports() const {
    return regular_imports_;
  }

  [[nodiscard]] std::set<std::filesystem::path> get_namespaces_imports() const {
    return namespaces_imports_;
  }

 private:
  std::map<std::filesystem::path, std::set<ImportName>> regular_imports_;
  std::set<std::filesystem::path> namespaces_imports_;
};
}  // namespace toolman
#endif  // TOOLMAN_IMPORT_H_
