// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_WALKER_H_
#define TOOLMAN_WALKER_H_

#include <algorithm>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "ToolmanLexer.h"
#include "ToolmanParserBaseListener.h"
#include "src/api.h"
#include "src/custom_type.h"
#include "src/document.h"
#include "src/error.h"
#include "src/field.h"
#include "src/import.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/scope.h"

namespace toolman {

class Compiler;

template <typename NODE, typename SOURCE>
StmtInfo get_stmt_info(NODE* node, SOURCE&& source) {
  auto id_start_token = node->getStart();
  return StmtInfo(
      {id_start_token->getLine(), node->getStop()->getLine()},
      {id_start_token->getStartIndex(), id_start_token->getStopIndex()},
      std::forward<SOURCE>(source));
}

class ImportBuilder {
 public:
  void start_import(std::string filename) {
    current_filename_ = std::move(filename);
  }

  void end_import() {
    if (current_import_name_.has_value()) {
      current_import_names_.push_back(current_import_name_.value());
    }
    if (is_star_) {
      import_.add_import_star(current_filename_);
    } else {
      import_.add_import(current_filename_, std::move(current_import_names_));
    }
    current_import_names_.clear();
  }

  void start_import_name(std::string import_name) {
    if (current_import_name_.has_value()) {
      current_import_names_.push_back(current_import_name_.value());
    }
    current_import_name_ = std::make_optional<ImportName>(
        ImportName{std::move(import_name), std::nullopt});
  }

  void start_import_name_alias(std::string alias_name) {
    current_import_name_.value().original_name = std::move(alias_name);
  }

  Import import() { return std::move(import_); }

  void set_import_star(bool is_star) { is_star_ = is_star; }

 private:
  std::string current_filename_;
  std::vector<ImportName> current_import_names_;
  std::optional<ImportName> current_import_name_;
  bool is_star_ = false;
  Import import_;
};

// Declare phase
class DeclPhaseWalker final : public ToolmanParserBaseListener,
                              public HasMultiError {
 public:
  DeclPhaseWalker(std::shared_ptr<std::filesystem::path> source,
                  Compiler* compiler)
      : type_scope_(std::make_shared<TypeScope>()),
        option_scope_(std::make_shared<OptionScope>()),
        source_(std::move(source)),
        compiler_(compiler) {
    buildin::decl_buildin_option(option_scope_.get());
  }

  void enterImportStatement(
      ToolmanParser::ImportStatementContext* node) override;

  void exitImportStatement(ToolmanParser::ImportStatementContext*) override;

  void enterFromImport(ToolmanParser::FromImportContext* node) override;

  void enterFromImportStar(ToolmanParser::FromImportStarContext*) override;

  void enterImportName(ToolmanParser::ImportNameContext* node) override;

  void enterImportNameAlias(
      ToolmanParser::ImportNameAliasContext* node) override;

  void enterStructDecl(ToolmanParser::StructDeclContext* node) override {
    decl_type<ToolmanParser::StructDeclContext, StructType>(node);
  }

  void enterEnumDecl(ToolmanParser::EnumDeclContext* node) override {
    decl_type<ToolmanParser::EnumDeclContext, EnumType>(node);
  }

  [[nodiscard]] const std::shared_ptr<TypeScope>& type_scope() const {
    return type_scope_;
  }

  [[nodiscard]] const std::shared_ptr<OptionScope>& option_scope() const {
    return option_scope_;
  }

  [[nodiscard]] Compiler* compiler() const { return compiler_; }

 private:
  template <typename NODE, typename DECL_TYPE>
  void decl_type(NODE* node) {
    StmtInfo stmt_info = get_stmt_info(node->identifierName(), source_);
    if (auto search = type_scope_->lookup(node->identifierName()->getText());
        search.has_value()) {
      push_error(DuplicateTypeDeclError(search.value(), stmt_info));
      return;
    } else {
      type_scope_->declare(std::make_shared<DECL_TYPE>(
          DECL_TYPE(node->identifierName()->getText(), stmt_info)));
    }
  }

  Import import() { return import_builder_.import(); }

  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  std::shared_ptr<std::filesystem::path> source_;
  ImportBuilder import_builder_;
  Compiler* compiler_;
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
      auto current_field = current_field_.value();
      if (auto field_opt =
              current_custom_type_->get_field_by_name(current_field.get_name());
          field_opt.has_value()) {
        throw DuplicateFieldDeclError(field_opt.value(),
                                      current_field.get_stmt_info());
      } else {
        current_custom_type_->append_field(current_field);
      }
      clear_current_field();
    }
  }

  void set_current_field_type(std::shared_ptr<Type> type) {
    if (current_field_.has_value()) {
      current_field_.value().set_type(std::move(type));
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

class ApiBuilder {
 public:
  void start_api_group(std::string group_name) {
    api_group_ = std::make_optional<ApiGroup>(std::move(group_name));
  }

  ApiGroup end_api_group() { return api_group_.value(); }

  void start_api(Api::HttpMethod http_method,
                 std::shared_ptr<Type> body_param) {
    api_ = std::make_optional(Api(http_method, std::move(body_param)));
  }

  void end_api() {
    if (api_group_.has_value() && api_.has_value()) {
      api_group_.value().add_api(api_.value());
      api_ = std::nullopt;
    }
  }

  void start_field(Field field) {
    current_field_ = std::optional<Field>(std::move(field));
  }

  void clear_current_field() { current_field_ = std::nullopt; }

  void end_field() {
    if (!api_.has_value()) {
      return;
    }
    auto api = api_.value();
    if (current_field_.has_value()) {
      auto current_field = current_field_.value();
      if (auto param_opt = api.get_path_param_by_name(current_field.get_name());
          param_opt.has_value()) {
        throw DuplicatePathParamDeclError(current_field,
                                          current_field.get_stmt_info());
      } else {
        api.add_path_param(
            PathParam{current_field_.value(), current_path_.length()});
      }
      clear_current_field();
    }
  }

  void append_path(const std::string& partial_path) {
    current_path_.append(partial_path);
  }

  void end_path() {
    api_->set_path(current_path_);
    current_path_.clear();
  }

  void insert_api_return(ApiReturn api_return) {
    api_->insert_api_return(std::move(api_return));
  }

 private:
  std::optional<Field> current_field_;
  std::string current_path_;
  std::optional<Api> api_;
  std::optional<ApiGroup> api_group_;
};

class RefPhaseWalker final : public ToolmanParserBaseListener,
                             public HasMultiError {
 public:
  enum class BuildState : char {
    IN_STRUCT,
    IN_ONEOF,
    RECURSIVE_ONFOF,
    IN_API_PATH_PARAM
  };

  RefPhaseWalker(std::shared_ptr<TypeScope> type_scope,
                 std::shared_ptr<OptionScope> option_scope,
                 std::shared_ptr<std::filesystem::path> source)
      : type_scope_(std::move(type_scope)),
        option_scope_(std::move(option_scope)),
        source_(std::move(source)),
        enum_builder_(),
        build_state_(BuildState::IN_STRUCT) {}
  std::unique_ptr<Document> get_document() {
    return std::unique_ptr<Document>(document_.release());
  }

  void enterDocument(ToolmanParser::DocumentContext* node) override {
    document_ = std::make_unique<Document>();
    document_->set_source(source_);
  }

  void enterOptionStatement(
      ToolmanParser::OptionStatementContext* node) override {
    auto search_opt = option_scope_->lookup(node->identifierName()->getText());
    if (!search_opt.has_value()) {
      push_error(
          UnknownOptionError(node->identifierName()->getText(),
                             get_stmt_info(node->identifierName(), source_)));
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
        push_error(OptionTypeMismatchError(
            search.get(), get_stmt_info(option_value_node, source_)));
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
    std::vector<std::string> comments;
    for (auto& dc : node->DocumentComment()) {
      comments.push_back(dc->getText().substr(3));
    }
    for (auto& ic : node->InlineComment()) {
      comments.push_back(ic->getText().substr(3, ic->getText().size() - 6));
    }
    auto field = Field(node->identifierName()->getText(),
                       get_stmt_info(node, source_), comments);
    field.set_optional(node->QuestionMark() != nullptr);
    if (build_state_ == BuildState::IN_STRUCT) {
      struct_builder_.start_field(field);
    } else if (build_state_ == BuildState::IN_ONEOF) {
      oneof_builder_.start_field(field);
    } else if (build_state_ == BuildState::IN_API_PATH_PARAM) {
      api_builder_.start_field(field);
    }
  }

  void exitStructField(ToolmanParser::StructFieldContext* node) override {
    try {
      if (build_state_ == BuildState::IN_STRUCT) {
        struct_builder_.end_field();
      } else if (build_state_ == BuildState::IN_ONEOF) {
        oneof_builder_.end_field();
      } else if (build_state_ == BuildState::IN_API_PATH_PARAM) {
        api_builder_.end_field();
      }
    } catch (DuplicateFieldDeclError& e) {
      push_error(e);
    }
  }

  void enterFieldType(ToolmanParser::FieldTypeContext* node) override {
    field_type_builder_.set_type_location(FieldTypeBuilder::TypeLocation::Top);
  }

  void enterListType(ToolmanParser::ListTypeContext* node) override {
    field_type_builder_.start_type(
        std::make_shared<ListType>(ListType(get_stmt_info(node, source_))));
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
          std::make_shared<MapType>(MapType(get_stmt_info(node, source_))));
    } catch (MapKeyTypeMustBePrimitiveError& e) {
      push_error(e);
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
        PrimitiveType(type_kind, get_stmt_info(node, source_))));
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
      push_error(CustomTypeNotFoundError(node->identifierName()->getText(),
                                         get_stmt_info(node, source_)));
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
    std::vector<std::string> comments;
    for (auto& dc : node->DocumentComment()) {
      comments.push_back(dc->getText().substr(3));
    }

    for (auto& ic : node->InlineComment()) {
      comments.push_back(ic->getText().substr(3, ic->getText().size() - 6));
    }

    auto enum_field = EnumField(node->identifierName()->getText(),
                                get_stmt_info(node, source_), comments);

    auto value = std::stoi(node->intgerLiteral()->getText());
    if (!enum_field.set_value(value)) {
      push_error(
          DuplicateEnumFieldValueError(EnumField::get_by_value(value).value(),
                                       get_stmt_info(node, source_)));
      return;
    }
    enum_builder_.start_field(enum_field);
  }

  void exitEnumField(ToolmanParser::EnumFieldContext* node) override {
    try {
      enum_builder_.end_field();
    } catch (DuplicateFieldDeclError& e) {
      push_error(e);
    }
  }

  void enterOneofType(ToolmanParser::OneofTypeContext* node) override {
    if (build_state_ == BuildState::IN_ONEOF) {
      push_error(RecursiveOneofTypeError(get_stmt_info(node, source_)));
      build_state_ = BuildState::RECURSIVE_ONFOF;
      return;
    }
    build_state_ = BuildState::IN_ONEOF;
    oneof_builder_.start_custom_type(
        std::make_shared<OneofType>(OneofType(get_stmt_info(node, source_))));
  }

  void exitOneofType(ToolmanParser::OneofTypeContext*) override {
    if (build_state_ == BuildState::IN_ONEOF) {
      struct_builder_.set_current_field_type(oneof_builder_.end_custom_type());
    }
    build_state_ = BuildState::IN_STRUCT;
  }

  void enterPathParam(ToolmanParser::PathParamContext*) override {
    build_state_ = BuildState::IN_API_PATH_PARAM;
  }

  void enterApiDecl(ToolmanParser::ApiDeclContext* node) override {
    api_builder_.start_api_group(node->identifierName()->getText());
  }

  void exitApiDecl(ToolmanParser::ApiDeclContext*) override {
    document_->insert_api_group(api_builder_.end_api_group());
  }

  void enterSingleApiDecl(ToolmanParser::SingleApiDeclContext* node) override {
    Api::HttpMethod http_method;
    switch (node->httpMethod()->getStart()->getType()) {
      case ToolmanLexer::Get:
        http_method = Api::HttpMethod::GET;
        break;
      case ToolmanLexer::Post:
        http_method = Api::HttpMethod::POST;
        break;
      case ToolmanLexer::Delete:
        http_method = Api::HttpMethod::DELETE;
        break;
      case ToolmanLexer::Put:
        http_method = Api::HttpMethod::PUT;
        break;
      case ToolmanLexer::Patch:
        http_method = Api::HttpMethod::PATCH;
        break;
      case ToolmanLexer::Head:
        http_method = Api::HttpMethod::HEAD;
        break;
      case ToolmanLexer::Options:
        http_method = Api::HttpMethod::OPTIONS;
        break;
      case ToolmanLexer::Trace:
        http_method = Api::HttpMethod::TRACE;
        break;
      case ToolmanLexer::Connect:
        http_method = Api::HttpMethod::CONNECT;
        break;
    }

    auto api_body_param_ident = node->identifierName();
    auto api_body_param_opt =
        type_scope_->lookup(api_body_param_ident->getText());
    if (!api_body_param_opt.has_value()) {
      push_error(CustomTypeNotFoundError(
          api_body_param_ident->getText(),
          get_stmt_info(api_body_param_ident, source_)));
      return;
    }
    api_builder_.start_api(http_method, api_body_param_opt.value());
  }

  void exitSingleApiDecl(ToolmanParser::SingleApiDeclContext*) override {
    api_builder_.end_api();
  }

  void enterPath(ToolmanParser::PathContext* node) override {
    for (auto child : node->children) {
      if (child == nullptr) {
        continue;
      }
      if (typeid(child) == typeid(antlr4::tree::TerminalNode*)) {
        auto slash = dynamic_cast<antlr4::tree::TerminalNode*>(child);
        api_builder_.append_path(slash->getText());
      } else if (typeid(child) == typeid(ToolmanParser::PathStringContext*)) {
        auto path_string =
            dynamic_cast<ToolmanParser::PathStringContext*>(child);
        api_builder_.append_path(path_string->getText());
      }
    }
  }

  void exitPath(ToolmanParser::PathContext*) override {
    api_builder_.end_path();
  }

  void enterReturnsItem(ToolmanParser::ReturnsItemContext* node) override {
    auto status_code = std::stoi(node->DecIntegerLiteral()->getText());
    std::shared_ptr<Type> return_type;
    if (nullptr == node->identifierName()) {
      auto return_type_opt =
          type_scope_->lookup(node->identifierName()->getText());
      if (!return_type_opt.has_value()) {
        push_error(CustomTypeNotFoundError(node->identifierName()->getText(),
                                           get_stmt_info(node, source_)));
        return;
      }
      return_type = return_type_opt.value();
    }
    api_builder_.insert_api_return(ApiReturn{status_code, return_type});
  }

 private:
  std::unique_ptr<Document> document_;
  CustomTypeBuilder<Field> struct_builder_;
  FieldTypeBuilder field_type_builder_;
  CustomTypeBuilder<EnumField> enum_builder_;
  CustomTypeBuilder<Field> oneof_builder_;
  std::shared_ptr<TypeScope> type_scope_;
  std::shared_ptr<OptionScope> option_scope_;
  std::shared_ptr<std::filesystem::path> source_;
  BuildState build_state_;
  ApiBuilder api_builder_;
};

}  // namespace toolman

#endif  // TOOLMAN_WALKER_H_
