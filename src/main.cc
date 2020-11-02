// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "src/compiler.h"
#include "src/generator.h"

int main(int argc, char **argv) {
  std::string filename = "/Users/ty/Desktop/toolman_examples.tm";  // for debug
  toolman::generator::TargetLanguage target =
      toolman::generator::target_language_from_string("");

  if (argc > 1) {
    if (argc == 3) {
      target = toolman::generator::target_language_from_string(argv[1]);
      filename = argv[2];
    } else {
      filename = argv[1];
    }
  }

  toolman::Compiler compiler;
  auto compile_res = compiler.compile(filename);

  for (const auto &error : compile_res.get_errors()) {
    std::cout << error.error() << std::endl << std::endl;
  }

  if (compile_res.has_fatal_error()) {
    return 1;
  }

  toolman::generator::generate(compile_res.get_document(), target, std::cout);
  return 0;
}
