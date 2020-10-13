// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_GENERATOR_H_
#define TOOLMAN_GENERATOR_H_

#include <memory>

#include "src/custom_type.h"
#include "src/document.h"

#define NL "\n"

namespace toolman {

class Generator {
 public:
  virtual void before_generate_struct(std::ostream& ostream,
                                      const Document* document) const {}
  virtual void after_generate_struct(std::ostream& ostream,
                                     const Document* document) const {}
  virtual void before_generate_enum(std::ostream& ostream,
                                    const Document* document) const {}
  virtual void after_generate_enum(std::ostream& ostream,
                                   const Document* document) const {}

  virtual void generate_struct(
      std::ostream& ostream,
      const std::shared_ptr<StructType>& struct_type) const = 0;
  virtual void generate_enum(
      std::ostream& ostream,
      const std::shared_ptr<EnumType>& enum_type) const = 0;

  void generate(std::ostream& ostream,
                const std::unique_ptr<Document>& document) const {
    before_generate_struct(ostream, document.get());
    for (const auto& struct_type : document->get_struct_types()) {
      generate_struct(ostream, struct_type);
    }

    after_generate_struct(ostream, document.get());
    before_generate_enum(ostream, document.get());
    for (const auto& enum_type : document->get_enum_types()) {
      generate_enum(ostream, enum_type);
    }
    after_generate_enum(ostream, document.get());
    ostream << std::flush;
  }
};
}  // namespace toolman

#endif  // TOOLMAN_GENERATOR_H_
