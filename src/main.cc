// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include <iostream>
#include <memory>
#include <string>
#include "src/compiler.h"

int main(int, char **) {
  std::string filename = "/Users/ty/Desktop/toolman_examples.tm";
  toolman::Compiler compiler;
  auto compile_res = compiler.compile(filename);

  for (const auto &error : compile_res.get_errors()) {
    std::cout << error.error() << std::endl << std::endl;
  }

  if (compile_res.has_fatal_error()) {
    return 1;
  }

  toolman::Compiler::generate(compile_res.get_document(),
                              toolman::Compiler::TargetLanguage::JAVA,
                              std::cout);

  return 0;
}
