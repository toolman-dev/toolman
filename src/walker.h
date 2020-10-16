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

#include "ToolmanLexer.h"
#include "ToolmanParserBaseListener.h"
#include "src/custom_type.h"
#include "src/document.h"
#include "src/error.h"
#include "src/field.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/scope.h"

namespace toolman {

template <typename NODE, typename FILE>
StmtInfo get_stmt_info(NODE* node, FILE&& file) {
  auto id_start_token = node->getStart();
  return StmtInfo(
      {id_start_token->getLine(), node->getStop()->getLine()},
      {id_start_token->getStartIndex(), id_start_token->getStopIndex()},
      std::forward<FILE>(file));
}

// Declare phase
class DeclPhaseWalker final : public ToolmanParserBaseListener {
 public:
  explicit DeclPhaseWalker(std::shared_ptr<std::filesystem::path> file)
      : type_scope_(std::make_shared<TypeScope>()),
        option_scope_(std::make_shared<OptionScope>()),
        file_(std::move(file)) {
      option_scope_->declare(std::make_shared<BoolOption>("use_java8_optional"));
      option_scope_->declare(std::make_shared<StringOption>("java_package"));
  }

  void enterStructDecl(ToolmanParser::StructDeclContext* node) override {
    decl_type<ToolmanParser::StructDeclContext, StructType>(node);
  }

  void enterEnumDecl(ToolmanParser::EnumDeclContext* node) override {
    decl_type<ToolmanParser::EnumDeclContext, EnumType>(node);
  }

  [[nodiscard]] const std::shared_ptr<TypeScope>& get_type_scope() const {
    return type_scope_;
  }

  [[nodiscard]] const std::shared_ptr<OptionScope>& get_option_scope() const {
    return option_scope_;
  }

  [[nodiscard]] const std::vector<Error>& get_errors() const { return errors_; }

 private:
  template <typename NODE, typename DECL_TYPE>
  void decl_type(NODE* node) {
    StmtInfo stmt_info = get_stmt_info(node->identifierName(), file_);
    if (auto search = type_scope_->lookup(node->identifierName()->getText());
        search.has_value()) {
      errors_.emplace_back(DuplicateTypeDeclError(search.value(), stmt_info));
      return;
    } else {
      type_scope_->declare(std::make_shared<DECL_TYPE>(
          DECL_TYPE(node->identifierName()->getText(), stmt_info,
                    node->Pub() != nullptr)));
    }
  }

  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  std::vector<Error> errors_;
  std::shared_ptr<std::filesystem::path> file_;
};

class FieldTypeBuilder {
 public:
  enum class TypeLocation : char { Top, ListElement, MapKey, MapValue };

  void set_type_location(TypeLocation type_location) {
    current_type_location_ = type_location;
  }

  void start_type(const std::shared_ptr<Type>& type);

  // If return value is not null-pointer
  // that means returned is current filed type
  std::shared_ptr<Type> end_map_or_list_type();

  // Other types besides map and list.
  // If return value is not null-pointer
  // that means returned is current filed type
  std::shared_ptr<Type> end_single_type();

 private:
  std::stack<std::shared_ptr<Type>> type_stack_;
  std::shared_ptr<Type> current_single_type_;
  TypeLocation current_type_location_ = TypeLocation::Top;
};

template <typename FIELD>
class CustomTypeBuilder {
 public:
  CustomTypeBuilder() : current_field_(std::nullopt) {}

  void start_custom_type(std::shared_ptr<CustomType<FIELD>> custom_type) {
    current_custom_type_ = std::move(custom_type);
  }

  [[nodiscard]] std::shared_ptr<CustomType<FIELD>> end_custom_type() {
    auto ret = current_custom_type_;
    current_custom_type_.reset();
    return ret;
  }

  void start_field(FIELD field) {
    current_field_ = std::optional<FIELD>(std::move(field));
  }

  void clear_current_field() { current_field_ = std::nullopt; }

  void end_field() {
    if (current_field_.has_value()) {
      if (!current_custom_type_->append_field(current_field_.value())) {
        auto current_field = current_field_.value();
        throw DuplicateFieldDeclError(
            current_custom_type_->get_field_by_name(current_field.get_name())
                .value(),
            current_field.get_stmt_info());
      }
      clear_current_field();
    }
  }

  void set_current_field_type(std::shared_ptr<Type> type) {
    if (current_field_.has_value()) {
      current_field_.value().set_type(std::move(type));
    }
  }

  void set_current_field_optional(bool optional) {
    if (current_field_.has_value()) {
      current_field_.value().set_optional(optional);
    }
  }

  std::shared_ptr<Type> get_current_field_type() {
    if (current_field_.has_value()) {
      return current_field_.value().get_type();
    }
    return std::shared_ptr<Type>(nullptr);
  }

 private:
  std::optional<FIELD> current_field_;
  std::shared_ptr<CustomType<FIELD>> current_custom_type_;
};

class RefPhaseWalker final : public ToolmanParserBaseListener {
 public:
  enum class BuildState : char { IN_STRUCT, IN_ONEOF, RECURSIVE_ONFOF };

  RefPhaseWalker(std::shared_ptr<TypeScope> type_scope,
                 std::shared_ptr<OptionScope> option_scope,
                 std::shared_ptr<std::filesystem::path> file)
      : type_scope_(std::move(type_scope)),
        option_scope_(std::move(option_scope)),
        file_(std::move(file)),
        enum_builder_(),
        build_state_(BuildState::IN_STRUCT) {}
  std::unique_ptr<Document> get_document() {
    return std::unique_ptr<Document>(document_.release());
  }

  [[nodiscard]] const std::vector<Error>& get_errors() const { return errors_; }

  void enterDocument(ToolmanParser::DocumentContext* node) override {
    document_ = std::make_unique<Document>();
    document_->set_file(file_);
  }

  void enterOptionStatement(
      ToolmanParser::OptionStatementContext* node) override {
    auto search_opt = option_scope_->lookup(node->identifierName()->getText());
    if (!search_opt.has_value()) {
      errors_.push_back(
          UnknownOptionError(node->identifierName()->getText(),
                             get_stmt_info(node->identifierName(), file_)));
    } else {
      auto search = search_opt.value();
      auto option_value_node = node->optionValue();
      if (option_value_node->BooleanLiteral() != nullptr && search->is_bool()) {
        auto bool_option = std::dynamic_pointer_cast<BoolOption>(search);
        bool_option->set_value(option_value_node->BooleanLiteral()->getText() ==
                               "true");
        document_->insert_option(bool_option);
      } else if (option_value_node->StringLiteral() != nullptr &&
                 search->is_string()) {
        auto string_option = std::dynamic_pointer_cast<StringOption>(search);
        string_option->set_value(option_value_node->StringLiteral()->getText());
        document_->insert_option(string_option);
      } else if (option_value_node->numericLiteral() != nullptr &&
                 search->is_numeric()) {
        auto numeric_option = std::dynamic_pointer_cast<NumericOption>(search);
        numeric_option->set_value(
            std::stod(option_value_node->numericLiteral()->getText()));
        document_->insert_option(numeric_option);
      } else {
        errors_.emplace_back(OptionTypeMismatchError(
            search.get(), get_stmt_info(option_value_node, file_)));
      }
    }
  }

  void enterStructDecl(ToolmanParser::StructDeclContext* node) override {
    auto type_name = node->identifierName()->getText();
    auto search_opt = type_scope_->lookup(node->identifierName()->getText());
    if (!search_opt.has_value()) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` not found.");
    }
    auto search = std::dynamic_pointer_cast<StructType>(search_opt.value());
    if (!search) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` is " +
                               search->to_string());
    }
    build_state_ = BuildState::IN_STRUCT;
    struct_builder_.start_custom_type(search);
  }

  void exitStructDecl(ToolmanParser::StructDeclContext* node) override {
    document_->insert_struct_type(std::dynamic_pointer_cast<StructType>(
        struct_builder_.end_custom_type()));
  }

  void enterStructField(ToolmanParser::StructFieldContext* node) override {
    auto field =
        Field(node->identifierName()->getText(), get_stmt_info(node, file_));
    if (build_state_ == BuildState::IN_STRUCT) {
      struct_builder_.start_field(field);
    } else if (build_state_ == BuildState::IN_ONEOF) {
      oneof_builder_.start_field(field);
    }
  }

  void exitStructField(ToolmanParser::StructFieldContext* node) override {
    try {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.set_current_field_optional(node->QuestionMark() !=
                                                   nullptr);
        struct_builder_.end_field();
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.set_current_field_optional(node->QuestionMark() !=
                                                  nullptr);
        oneof_builder_.end_field();
      }
    } catch (DuplicateFieldDeclError& e) {
      errors_.emplace_back(e);
    }
  }

  void enterFieldType(ToolmanParser::FieldTypeContext* node) override {
    field_type_builder_.set_type_location(FieldTypeBuilder::TypeLocation::Top);
  }

  void enterListType(ToolmanParser::ListTypeContext* node) override {
    field_type_builder_.start_type(
        std::make_shared<ListType>(ListType(get_stmt_info(node, file_))));
  }

  void exitListType(ToolmanParser::ListTypeContext*) override {
    if (auto type = field_type_builder_.end_map_or_list_type(); type) {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.set_current_field_type(type);
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.set_current_field_type(type);
      }
    }
  }

  void enterListElementType(ToolmanParser::ListElementTypeContext*) override {
    field_type_builder_.set_type_location(
        FieldTypeBuilder::TypeLocation::ListElement);
  }

  void enterMapType(ToolmanParser::MapTypeContext* node) override {
    try {
      field_type_builder_.start_type(
          std::make_shared<MapType>(MapType(get_stmt_info(node, file_))));
    } catch (MapKeyTypeMustBePrimitiveError& e) {
      errors_.emplace_back(e);
    }
  }

  void exitMapType(ToolmanParser::MapTypeContext*) override {
    if (auto type = field_type_builder_.end_map_or_list_type(); type) {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.set_current_field_type(type);
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.set_current_field_type(type);
      }
    }
  }

  void enterMapKeyType(ToolmanParser::MapKeyTypeContext*) override {
    field_type_builder_.set_type_location(
        FieldTypeBuilder::TypeLocation::MapKey);
  }
  void enterMapValueType(ToolmanParser::MapValueTypeContext*) override {
    field_type_builder_.set_type_location(
        FieldTypeBuilder::TypeLocation::MapValue);
  }

  void enterPrimitiveType(ToolmanParser::PrimitiveTypeContext* node) override {
    PrimitiveType::TypeKind type_kind;
    if (node->Bool() != nullptr) {
      type_kind = PrimitiveType::TypeKind::Bool;
    } else if (node->I32() != nullptr) {
      type_kind = PrimitiveType::TypeKind::I32;
    } else if (node->U32() != nullptr) {
      type_kind = PrimitiveType::TypeKind::U32;
    } else if (node->I64() != nullptr) {
      type_kind = PrimitiveType::TypeKind::I64;
    } else if (node->U64() != nullptr) {
      type_kind = PrimitiveType::TypeKind::U64;
    } else if (node->Float() != nullptr) {
      type_kind = PrimitiveType::TypeKind::Float;
    } else if (node->String() != nullptr) {
      type_kind = PrimitiveType::TypeKind::String;
    } else {
      type_kind = PrimitiveType::TypeKind::Any;
    }
    field_type_builder_.start_type(std::make_shared<PrimitiveType>(
        PrimitiveType(type_kind, get_stmt_info(node, file_))));
  }

  void exitPrimitiveType(ToolmanParser::PrimitiveTypeContext*) override {
    if (auto type = field_type_builder_.end_single_type(); type) {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.set_current_field_type(type);
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.set_current_field_type(type);
      }
    }
  }

  void enterCustomTypeName(
      ToolmanParser::CustomTypeNameContext* node) override {
    auto custom_type = type_scope_->lookup(node->identifierName()->getText());
    if (!custom_type.has_value()) {
      errors_.emplace_back(CustomTypeNotFoundError(
          node->identifierName()->getText(), get_stmt_info(node, file_)));
      return;
    }
    field_type_builder_.start_type(custom_type.value());
  }

  void exitCustomTypeName(ToolmanParser::CustomTypeNameContext*) override {
    if (auto type = field_type_builder_.end_single_type(); type) {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.set_current_field_type(type);
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.set_current_field_type(type);
      }
    }
  }

  void enterEnumDecl(ToolmanParser::EnumDeclContext* node) override {
    auto type_name = node->identifierName()->getText();
    auto search_opt = type_scope_->lookup(node->identifierName()->getText());
    if (!search_opt.has_value()) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` not found.");
    }
    auto search = std::dynamic_pointer_cast<EnumType>(search_opt.value());
    if (!search) {
      // Logically, this situation will not happen
      throw std::runtime_error("The type name`" + type_name + "` is " +
                               search->to_string());
    }
    enum_builder_.start_custom_type(search);
  }

  void exitEnumDecl(ToolmanParser::EnumDeclContext*) override {
    document_->insert_enum_type(
        std::dynamic_pointer_cast<EnumType>(enum_builder_.end_custom_type()));
  }

  void enterEnumField(ToolmanParser::EnumFieldContext* node) override {
    auto enum_field = EnumField(node->identifierName()->getText(),
                                get_stmt_info(node, file_));
    auto value = std::stoi(node->intgerLiteral()->getText());
    if (!enum_field.set_value(value)) {
      errors_.emplace_back(DuplicateEnumFieldValueError(
          EnumField::get_by_value(value).value(), get_stmt_info(node, file_)));
      return;
    }
    enum_builder_.start_field(enum_field);
  }

  void exitEnumField(ToolmanParser::EnumFieldContext* node) override {
    try {
      enum_builder_.end_field();
    } catch (DuplicateFieldDeclError& e) {
      errors_.emplace_back(e);
    }
  }

  void enterOneofType(ToolmanParser::OneofTypeContext* node) override {
    if (build_state_ == BuildState::IN_ONEOF) {
      errors_.emplace_back(RecursiveOneofTypeError(get_stmt_info(node, file_)));
      build_state_ = BuildState::RECURSIVE_ONFOF;
      return;
    }
    build_state_ = BuildState::IN_ONEOF;
    oneof_builder_.start_custom_type(
        std::make_shared<OneofType>(OneofType(get_stmt_info(node, file_))));
  }

  void exitOneofType(ToolmanParser::OneofTypeContext*) override {
    if (build_state_ == BuildState::IN_ONEOF) {
      struct_builder_.set_current_field_type(oneof_builder_.end_custom_type());
    }
    build_state_ = BuildState::IN_STRUCT;
  }

 private:
  std::unique_ptr<Document> document_;
  CustomTypeBuilder<Field> struct_builder_;
  FieldTypeBuilder field_type_builder_;
  CustomTypeBuilder<EnumField> enum_builder_;
  CustomTypeBuilder<Field> oneof_builder_;
  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  std::shared_ptr<std::filesystem::path> file_;
  std::vector<Error> errors_;
  BuildState build_state_;
};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
