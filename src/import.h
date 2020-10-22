// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_IMPORT_H_
#define TOOLMAN_IMPORT_H_

#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

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
    if (auto import_name_set = regular_imports_.find(filename);
        import_name_set != regular_imports_.end()) {
      import_name_set->second.insert(
          std::make_move_iterator(import_names.begin()),
          std::make_move_iterator(import_names.end()));
    } else {
      // filename not found
      regular_imports_.emplace(
          filename,
          std::set<ImportName>(std::make_move_iterator(import_names.begin()),
                               std::make_move_iterator(import_names.end())));
    }
  }

  // from 'xxx.tm' import *
  void add_import_star(const std::string& filename) {
    namespaces_imports_.emplace(filename);
  }

  [[nodiscard]] std::map<std::string, std::set<ImportName>>
  get_regular_imports() const {
    return regular_imports_;
  }

  [[nodiscard]] std::set<std::string> get_namespaces_imports() const {
    return namespaces_imports_;
  }

 private:
  std::map<std::string, std::set<ImportName>> regular_imports_;
  std::set<std::string> namespaces_imports_;
};
}  // namespace toolman
#endif  // TOOLMAN_IMPORT_H_
