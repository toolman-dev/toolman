// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_IMPORT_H_
#define TOOLMAN_IMPORT_H_

#include <optional>
#include <string>
#include <vector>

namespace toolman {

struct ImportName {
  std::string original_name;
  std::optional<std::string> local_name;
};

class Import {
 public:
  void add_import(std::string filename, std::vector<ImportName> import_names) {

  }

  void add_import_star(std::string filename) {

  }
};
}  // namespace toolman
#endif  // TOOLMAN_IMPORT_H_
