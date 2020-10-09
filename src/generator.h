// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_GENERATOR_H_
#define TOOLMAN_GENERATOR_H_

#include "src/custom_type.h"

namespace toolman {
class Generator {
 public:
  virtual void generate_struct(const StructType& struct_type) = 0;
  virtual void generate_enum(const EnumType& enum_type) = 0;

  void generate(const Document& document) const {
    for (const auto& struct_type : document.get_struct_types()) {
      generate_struct(struct_type);
    }

    for (const auto& enum_type : document.get_enum_types()) {
      generate_enum(enum_type);
    }
  }
};
}  // namespace toolman

#endif  // TOOLMAN_GENERATOR_H_
