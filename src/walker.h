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
      : type_scope_(std::make_shared<Scope>(Scope())), file_(std::move(file)) {}
  void enterStructDecl(ToolmanParser::StructDeclContext* node) override {
    decl_type<ToolmanParser::StructDeclContext, StructType>(node);
  }

  void enterEnumDecl(ToolmanParser::EnumDeclContext* node) override {
    decl_type<ToolmanParser::EnumDeclContext, EnumType>(node);
  }

  [[nodiscard]] const std::shared_ptr<Scope>& get_type_scope() const {
    return type_scope_;
  }

  [[nodiscard]] const std::vector<Error>& get_errors() const { return errors_; }

 private:
  std::shared_ptr<Scope> type_scope_;
  std::vector<Error> errors_;
  std::shared_ptr<std::string> file_;

  template <typename NODE, typename DECL_TYPE>
  void decl_type(NODE* node) {
    auto id_start_token = node->identifierName()->getStart();
    auto stmt_info = StmtInfo(
        {id_start_token->getLine(),
         node->identifierName()->getStop()->getLine()},
        {id_start_token->getStartIndex(), id_start_token->getStopIndex()},
        file_);
    if (auto search =
            type_scope_->lookup_type(node->identifierName()->getText());
        search.has_value()) {
      errors_.emplace_back(DuplicateDeclError(search.value(), stmt_info));
      return;
    } else {
      type_scope_->declare(std::make_shared<DECL_TYPE>(
          DECL_TYPE(node->identifierName()->getText(), stmt_info,
                    node->Pub() != nullptr)));
    }
  }
};

class RefPhaseWalker final : public ToolmanParserBaseListener {};

class FieldTypeBuilder {
  enum class TypeLocation : char { Top, ListElement, MapKey, MapValue };

  void start_type(const std::shared_ptr<Type>& type, TypeLocation type_location) {
    if (TypeLocation::ListElement == type_location &&
        type_stack_.top()->is_list()) {
      auto list_type = std::dynamic_pointer_cast<ListType>(type_stack_.top());
      list_type->set_elem_type(type);
    }

    if (type_stack_.top()->is_map()) {
      auto map_type = std::dynamic_pointer_cast<MapType>(type_stack_.top());

      if (TypeLocation::MapKey == type_location) {
        // The key of the map must be a primitive type.
        if (!type->is_primitive()) {
          errors_.emplace_back(MapKeyTypeMustBePrimitiveError(type));
        } else {
          map_type->set_key_type(
              std::dynamic_pointer_cast<PrimitiveType>(type));
        }
      }

      if (TypeLocation::MapValue == type_location) {
        map_type->set_value_type(
            std::dynamic_pointer_cast<PrimitiveType>(type));
      }
    }

    if (type->is_map() || type->is_list()) {
      type_stack_.push(type);
    } else {
      current_type_ = type;
    }
  }

  std::shared_ptr<Type> end_map_or_list_type() {
    auto top = type_stack_.top();
    type_stack_.pop();
    return top;
  }

  // Other types besides map and list.
  std::shared_ptr<Type> end_single_type() { return current_type_; }

 private:
  std::stack<std::shared_ptr<Type>> type_stack_;
  std::shared_ptr<Type> current_type_;
  std::vector<Error> errors_;
};

template <typename FIELD, typename CUSTOM_TYPE>
class CustomTypeBuilder {
 public:
  explicit CustomTypeBuilder(std::shared_ptr<Scope> type_scope)
      : type_scope_(std::move(type_scope)) {}
  void append_field(FIELD f) { current_struct_type_->append_field(f); }

  void start_build_type(const std::string& type_name) {
    auto search_opt = type_scope_->lookup_type(type_name);
    if (!search_opt.has_value()) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` not found.");
    }
    auto search = std::dynamic_pointer_cast<CUSTOM_TYPE>(search_opt.value());
    if (!search) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` is " +
                               search->to_string());
    }
    current_struct_type_ = search;
  }

 private:
  std::shared_ptr<CUSTOM_TYPE> current_struct_type_;
  std::shared_ptr<Scope> type_scope_;
};

class LiteralBuilder {
 public:
};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
