// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_DOC_H_
#define TOOLMAN_DOC_H_

#include <string>
#include <vector>

#include "src/custom_type.h"
#include "src/type.h"

namespace toolman {

class Document final {
 public:
  const std::vector<StructType>& get_struct_types() const {
    return struct_types_;
  }
  const std::vector<EnumType>& get_enum_types() const { return enum_types_; }

 private:
  std::vector<StructType> struct_types_;
  std::vector<EnumType> enum_types_;
};
}  // namespace toolman
#endif  // TOOLMAN_DOC_H_
