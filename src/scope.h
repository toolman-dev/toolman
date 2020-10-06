// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_SCOPE_TYPE_H_
#define TOOLMAN_SCOPE_TYPE_H_

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "src/type.h"

namespace toolman {
class Scope {
 public:
  // Lookup_type returns the type with the given name if it is
  // found in this scope, otherwise it returns std::nullopt.
  std::optional<std::shared_ptr<Type>> lookup_type(const std::string &name);

  // Declare a type into the scope.
  // If the scope did not have this scope present, `true` is returned.
  // If the map did have this key present, the value is updated,
  // and the `false` is returned.
  bool declare(std::shared_ptr<Type> type);

 private:
  // Map of names to types
  std::map<std::string, std::shared_ptr<Type>> types_;
};

}  // namespace toolman
#endif  // TOOLMAN_SCOPE_TYPE_H_
