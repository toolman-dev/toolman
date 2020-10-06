// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <string>
#include <vector>

#include "src/type.h"

namespace toolman {

class Document final {
 public:
  const std::vector<Type>& get_types() { return types_; }

 private:
  std::vector<Type> types_;
};
}  // namespace toolman
#endif  // TOOLMAN_DOC_H_
