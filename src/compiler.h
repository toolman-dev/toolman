// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_COMPILER_H_
#define TOOLMAN_COMPILER_H_

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "ToolmanLexer.h"
#include "ToolmanParser.h"
#include "src/error.h"
#include "src/generator.h"
#include "src/golang_generator.h"
#include "src/java_generator.h"
#include "src/typescript_generator.h"
#include "src/walker.h"

namespace toolman {

class Module : public HasMultiError {
 public:
  Module(std::shared_ptr<TypeScope> type_scope,
         std::shared_ptr<OptionScope> option_scope,
         antlr4::tree::ParseTree *parse_tree,
         std::shared_ptr<std::filesystem::path> source,
         std::vector<Error> errors)
      : type_scope_(type_scope),
        option_scope_(option_scope),
        parse_tree_(parse_tree),
        source_(source),
        HasMultiError(std::move(errors)) {}

  std::shared_ptr<TypeScope> get_type_scope() { return type_scope_; }

  std::shared_ptr<OptionScope> get_option_scope() { return option_scope_; }
  antlr4::tree::ParseTree *get_parse_tree() { return parse_tree_; }
  std::shared_ptr<std::filesystem::path> get_source() { return source_; }

 private:
  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  antlr4::tree::ParseTree *parse_tree_;
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
  enum class TargetLanguage : char { GOLANG, TYPESCRIPT, JAVA };

  static CompileResult compile(const std::string& src_path) {
    auto module = compile_module(src_path);

    auto ref_phase_walker =
        RefPhaseWalker(module->get_type_scope(), module->get_option_scope(),
                       module->get_source());

    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&ref_phase_walker,
                                                module->get_parse_tree());

    auto errors = module->get_errors();
    auto ref_phase_errors = ref_phase_walker.get_errors();
    errors.insert(errors.end(), ref_phase_errors.begin(),
                  ref_phase_errors.end());
    return CompileResult(ref_phase_walker.get_document(), errors);
  }

  static void generate(std::unique_ptr<Document> document,
                TargetLanguage targetLanguage, std::ostream &ostream) {
    std::unique_ptr<Generator> generator;
    switch (targetLanguage) {
      case TargetLanguage::GOLANG:
        generator = std::make_unique<GolangGenerator>();
        break;
      case TargetLanguage::TYPESCRIPT:
        generator = std::make_unique<TypescriptGenerator>();
        break;
      case TargetLanguage::JAVA:
        generator = std::make_unique<JavaGenerator>();
        break;
    }
    generator->generate(ostream, document);
  }

 private:
  static std::unique_ptr<Module> compile_module(const std::string& src_path) {
    std::unique_ptr<std::istream> istream;
    std::shared_ptr<std::filesystem::path> source =
        std::make_shared<std::filesystem::path>(src_path);

    auto ifs = std::make_unique<std::ifstream>(
        std::ifstream(*source, std::ios_base::in));

    antlr4::ANTLRInputStream input(*istream);
    ToolmanLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    tokens.fill();

    ToolmanParser parser(&tokens);

    antlr4::tree::ParseTree *tree = parser.document();

    auto def_phase_walker = toolman::DeclPhaseWalker(source);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&def_phase_walker, tree);

    return std::make_unique<Module>(def_phase_walker.get_type_scope(),
                                    def_phase_walker.get_option_scope(), tree,
                                    source, def_phase_walker.get_errors());
  }
};
}  // namespace toolman

#endif  // TOOLMAN_COMPILER_H_
