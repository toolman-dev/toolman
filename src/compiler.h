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
#include <vector>

#include "ToolmanLexer.h"
#include "ToolmanParser.h"
#include "src/error.h"
#include "src/walker.h"

namespace toolman {

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

  std::shared_ptr<TypeScope> type_scope() { return type_scope_; }

  std::shared_ptr<OptionScope> option_scope() { return option_scope_; }
  std::shared_ptr<std::filesystem::path> source() { return source_; }

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

  // Use shared_ptr as return value, Convenient to no longer use import class
  // later.
  std::shared_ptr<Module> compile_module(const std::string& src_path);

  CompileResult compile(const std::string& src_path);

 private:
  antlr4::tree::ParseTreeWalker walker_;
  std::map<std::filesystem::path, std::shared_ptr<Module>> modules_;
  std::filesystem::path base_path_;
};
}  // namespace toolman

#endif  // TOOLMAN_COMPILER_H_
