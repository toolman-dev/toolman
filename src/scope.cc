// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/scope.h"

#include <utility>

namespace toolman {

namespace buildin {
void decl_buildin_option(OptionScope* option_scope) {
  option_scope->declare(
      std::make_shared<
          std::remove_const_t<decltype(option_use_java8_optional)>>(
          option_use_java8_optional));
  option_scope->declare(
      std::make_shared<std::remove_const_t<decltype(option_java_package)>>(
          option_java_package));
}
}  // namespace buildin
}  // namespace toolman
