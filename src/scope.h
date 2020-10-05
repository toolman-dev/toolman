// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_SCOPE_TYPE_H_
#define TOOLMAN_SCOPE_TYPE_H_

#include <map>
#include <optional>
#include <memory>
#include <string>

#include "src/type.h"

namespace toolman {
class Scope {
 public:
    // Lookup_type returns the type with the given name if it is
    // found in this scope, otherwise it returns std::nullopt.
    std::optional <std::shared_ptr<Type>> lookup_type(const std::string &name);

    // Declare a type into the scope.
    std::optional <std::shared_ptr<Type>> declare(std::shared_ptr <Type> type);

 private:
    // Map of names to types
    std::map <std::string, std::shared_ptr<Type>> types_;
};

}  // namespace toolman
#endif  // TOOLMAN_SCOPE_TYPE_H_
