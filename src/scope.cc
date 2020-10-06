// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/scope.h"

#include <memory>
#include <optional>
#include <utility>

#include "src/type.h"

namespace toolman {

std::optional<std::shared_ptr<Type>> Scope::lookup_type(
    const std::string &name) {
  if (const auto it = types_.find(name); types_.end() != it) {
    return {it->second};
  }
  return std::nullopt;
}

bool Scope::declare(std::shared_ptr<Type> type) {
  auto res = types_.emplace(type->get_name(), std::move(type));
  return res.second;
}

}  // namespace toolman
