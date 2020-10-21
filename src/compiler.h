// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_COMPILER_H_
#define TOOLMAN_COMPILER_H_

#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "ToolmanLexer.h"
#include "ToolmanParser.h"
#include "src/error.h"
#include "src/walker.h"

namespace toolman {

#define DEF_PHASE_WALK(source, compiler)                     \
  auto ifs = std::ifstream(*(source), std::ios_base::in);    \
  antlr4::ANTLRInputStream input(ifs);                       \
  ToolmanLexer lexer(&input);                                \
  antlr4::CommonTokenStream tokens(&lexer);                  \
  tokens.fill();                                             \
  ToolmanParser parser(&tokens);                             \
  antlr4::tree::ParseTree *tree = parser.document();         \
  auto def_phase_walker = DeclPhaseWalker(source, compiler); \
  walker_.walk(&def_phase_walker, tree);

class Module : public HasMultiError {
 public:
  Module(std::shared_ptr<TypeScope> type_scope,
         std::shared_ptr<OptionScope> option_scope,
         std::shared_ptr<std::filesystem::path> source,
         std::vector<Error> errors)
      : type_scope_(std::move(type_scope)),
        option_scope_(std::move(option_scope)),
        source_(std::move(source)),
        HasMultiError(std::move(errors)) {}
  std::shared_ptr<TypeScope> get_type_scope() { return type_scope_; }

  std::shared_ptr<OptionScope> get_option_scope() { return option_scope_; }
  std::shared_ptr<std::filesystem::path> get_source() { return source_; }

 private:
  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  std::shared_ptr<std::filesystem::path> source_;
};

class CompileResult final : public HasMultiError {
 public:
  CompileResult(std::unique_ptr<Document> document, std::vector<Error> errors)
      : document_(std::move(document)), HasMultiError(std::move(errors)) {}

  std::unique_ptr<Document> get_document() {
    return std::unique_ptr<Document>(document_.release());
  }

 private:
  std::unique_ptr<Document> document_;
};

class Compiler {
 public:
  Compiler() : walker_(antlr4::tree::ParseTreeWalker::DEFAULT) {}

  void compile_module(const std::string &src_path) {
    auto source_ptr = std::make_shared<std::filesystem::path>(
        std::filesystem::absolute(src_path).lexically_normal());
    if (auto it = modules_.find(*source_ptr); it != modules_.end()) {
      return;
    }
    DEF_PHASE_WALK(source_ptr, this);
    walker_.walk(&def_phase_walker, tree);

    modules_.emplace(
        *source_ptr,
        std::make_unique<Module>(def_phase_walker.get_type_scope(),
                                 def_phase_walker.get_option_scope(),
                                 source_ptr, def_phase_walker.get_errors()));
  }

  CompileResult compile(const std::string &src_path) {
    auto source_ptr = std::make_shared<std::filesystem::path>(
        std::filesystem::absolute(src_path).lexically_normal());

    DEF_PHASE_WALK(source_ptr, this);

    auto ref_phase_walker =
        RefPhaseWalker(def_phase_walker.get_type_scope(),
                       def_phase_walker.get_option_scope(), source_ptr);

    walker_.walk(&ref_phase_walker, tree);

    auto errors = def_phase_walker.get_errors();
    auto ref_phase_errors = ref_phase_walker.get_errors();
    errors.insert(errors.end(), ref_phase_errors.begin(),
                  ref_phase_errors.end());
    return CompileResult(ref_phase_walker.get_document(), errors);
  }

 private:
  antlr4::tree::ParseTreeWalker walker_;
  std::map<std::filesystem::path, std::unique_ptr<Module>> modules_;
};
}  // namespace toolman

#endif  // TOOLMAN_COMPILER_H_
