// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <vector>

#include "ToolmanLexer.h"
#include "ToolmanParser.h"
#include "ToolmanParserBaseListener.h"
#include "src/error.h"
#include "src/golang_generator.h"
#include "src/java_generator.h"
#include "src/scope.h"
#include "src/walker.h"

using toolman::DeclPhaseWalker;
using toolman::RefPhaseWalker;
using toolman::ToolmanLexer;
using toolman::ToolmanParser;

int main(int, char **) {
  std::string filename = "/Users/ty/Desktop/toolman_examples.tm";
  std::ifstream tm_file(filename);
  if (tm_file.is_open()) {
    antlr4::ANTLRInputStream input(tm_file);
    ToolmanLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    tokens.fill();

    ToolmanParser parser(&tokens);
    antlr4::tree::ParseTree *tree = parser.document();

    antlr4::tree::ParseTreeWalker walker;
    auto def_phase_walker =
        DeclPhaseWalker(std::make_shared<std::filesystem::path>(filename));

    walker.walk(&def_phase_walker, tree);

    bool has_fetal = false;

    for (const auto &error : def_phase_walker.get_errors()) {
      if (!has_fetal && error.is_fatal()) {
        has_fetal = true;
      }
      std::cout << error.error() << std::endl << std::endl;
    }

    auto ref_phase_walker =
        RefPhaseWalker(def_phase_walker.get_type_scope(),
                       std::make_shared<std::filesystem::path>(filename));

    walker.walk(&ref_phase_walker, tree);

    for (const auto &error : ref_phase_walker.get_errors()) {
      if (!has_fetal && error.is_fatal()) {
        has_fetal = true;
      }
      std::cout << error.error() << std::endl << std::endl;
    }

    // std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
    if (!has_fetal) {
      toolman::JavaGenerator g;
      g.generate(std::cout, ref_phase_walker.get_document());
    }
  }
  return 0;
}
