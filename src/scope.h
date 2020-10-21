// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_SCOPE_H_
#define TOOLMAN_SCOPE_H_

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "src/option.h"
#include "src/type.h"

namespace toolman {
template <typename T>
class Scope {
 public:
  // Lookup returns the `T` with the given name if it is
  // found in this scope, otherwise it returns std::nullopt.
  std::optional<std::shared_ptr<T>> lookup(const std::string& name) {
    if (const auto it = data_.find(name); data_.end() != it) {
      return {it->second};
    }
    return std::nullopt;
  }

  // Declare a `T` into the scope.
  // If the scope did not have this scope present, `true` is returned.
  // If the map did have this key present, the value is updated,
  // and the `false` is returned.
  bool declare(std::shared_ptr<T> item) {
    auto res = data_.emplace(item->get_name(), std::move(item));
    return res.second;
  }

 private:
  // Map of names to `T`
  std::map<std::string, std::shared_ptr<T>> data_;
};

class TypeScope final : public Scope<Type> {};

class OptionScope final : public Scope<Option> {};

namespace buildin {
const auto option_use_java8_optional = BoolOption("use_java8_optional");
const auto option_java_package = StringOption("java_package");

void decl_buildin_option(OptionScope* option_scope);
}  // namespace buildin

}  // namespace toolman
#endif  // TOOLMAN_SCOPE_H_
