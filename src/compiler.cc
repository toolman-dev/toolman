// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "compiler.h"

#define DEF_PHASE_WALK(source, compiler)                     \
  auto ifs = std::ifstream(*(source), std::ios_base::in);    \
  if (!ifs.is_open()) {                                      \
    throw FileNotFoundError(source);                         \
  }                                                          \
  antlr4::ANTLRInputStream input(ifs);                       \
  ToolmanLexer lexer(&input);                                \
  antlr4::CommonTokenStream tokens(&lexer);                  \
  tokens.fill();                                             \
  ToolmanParser parser(&tokens);                             \
  antlr4::tree::ParseTree* tree = parser.document();         \
  auto def_phase_walker = DeclPhaseWalker(source, compiler); \
  walker_.walk(&def_phase_walker, tree);

namespace toolman {
std::shared_ptr<Module> Compiler::compile_module(const std::string& src_path) {
  auto source = std::filesystem::path(src_path).lexically_normal();
  if (source.is_relative()) {
    source = base_path_ / source;
  }
  if (auto it = modules_.find(source); it != modules_.end()) {
    return it->second;
  }
  auto source_ptr = std::make_shared<std::filesystem::path>(source);
  DEF_PHASE_WALK(source_ptr, this);
  walker_.walk(&def_phase_walker, tree);
  auto module = std::make_shared<Module>(
      def_phase_walker.type_scope(), def_phase_walker.option_scope(),
      source_ptr, def_phase_walker.get_errors());
  modules_.emplace(source, module);
  return module;
}

CompileResult Compiler::compile(const std::string& src_path) {
  auto source_ptr = std::make_shared<std::filesystem::path>(
      std::filesystem::absolute(src_path).lexically_normal());
  base_path_ = source_ptr->parent_path();
  DEF_PHASE_WALK(source_ptr, this);

  auto ref_phase_walker =
      RefPhaseWalker(def_phase_walker.type_scope(),
                     def_phase_walker.option_scope(), source_ptr);

  walker_.walk(&ref_phase_walker, tree);

  auto errors = def_phase_walker.get_errors();
  auto ref_phase_errors = ref_phase_walker.get_errors();
  errors.insert(errors.end(), ref_phase_errors.begin(), ref_phase_errors.end());
  return CompileResult(ref_phase_walker.get_document(), errors);
}
}  // namespace toolman
