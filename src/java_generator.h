// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_JAVA_GENERATOR_H_
#define TOOLMAN_JAVA_GENERATOR_H_

#include "src/generator.h"

namespace toolman {
class JavaGenerator : public Generator {
 public:
  void before_generate_document(std::ostream &ostream,
                                const Document *document) const override {
    auto outclass = capitalize(camelcase(document->get_file()->stem().string()));
    ostream << "public final class " << outclass << " {" << NL << INDENTATION
            << "private " << outclass << "() {}";
  }
  void after_generate_document(std::ostream &ostream,
                               const Document *document) const override {
    ostream << NL << "}";
  }

  void before_generate_enum(std::ostream &ostream,
                            const Document *document) const override {}

  void after_generate_enum(std::ostream &ostream,
                           const Document *document) const override {}
  void before_generate_struct(std::ostream &ostream,
                              const Document *document) const override {}

  void after_generate_struct(std::ostream &ostream,
                             const Document *document) const override {}

  void generate_struct(
      std::ostream &ostream,
      const std::shared_ptr<StructType> &struct_type) const override {}

  void generate_enum(
      std::ostream &ostream,
      const std::shared_ptr<EnumType> &enum_type) const override {}

 private:
};
}  // namespace toolman
#endif  // TOOLMAN_GOLANG_GENERATOR_H_
