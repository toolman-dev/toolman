// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_WALKER_H_
#define TOOLMAN_WALKER_H_

#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "ToolmanParserBaseListener.h"
#include "src/custom_type.h"
#include "src/error.h"
#include "src/field.h"
#include "src/list_type.h"
#include "src/map_type.h"
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

class FieldTypeBuilder {
  void startType(std::shared_ptr<Type> type) {
    if (type_stack_.top()->is_list()) {
      auto list_type = std::dynamic_pointer_cast<ListType>(type_stack_.top());
      list_type->set_elem_type(type);
    }

    if (type_stack_.top()->is_map()) {
      auto map_type = std::dynamic_pointer_cast<MapType>(type_stack_.top());
    }

    if (type->is_map() || type->is_list()) {
      type_stack_.push(type);
      return;
    }
  }

 private:
  std::stack<std::shared_ptr<Type>> type_stack_;
};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
