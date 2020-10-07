// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_WALKER_H_
#define TOOLMAN_WALKER_H_

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ToolmanParserBaseListener.h"
#include "src/custom_type.h"
#include "src/error.h"
#include "src/field.h"
#include "src/scope.h"

namespace toolman {

// Declare phase
class DeclPhaseWalker final : public ToolmanParserBaseListener {
 public:
  explicit DeclPhaseWalker(std::shared_ptr<std::string> file)
      : file_(std::move(file)) {}
  void enterStructDecl(ToolmanParser::StructDeclContext* node) override {
    decl_type<ToolmanParser::StructDeclContext, StructType>(node);
  }

  void enterEnumDecl(ToolmanParser::EnumDeclContext* node) override {
    decl_type<ToolmanParser::EnumDeclContext, EnumType>(node);
  }

  [[nodiscard]] const Scope& get_type_scope() const { return type_scope_; }

  [[nodiscard]] const std::vector<Error>& get_errors() const { return errors_; }

 private:
  Scope type_scope_;
  std::vector<Error> errors_;
  std::shared_ptr<std::string> file_;

  template <typename NODE, typename DECL_TYPE>
  void decl_type(NODE* node);
};

class RefPhaseWalker final : public ToolmanParserBaseListener {};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
