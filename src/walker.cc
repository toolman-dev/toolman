// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/walker.h"

#include <algorithm>

#include "src/compiler.h"

namespace toolman {

void DeclPhaseWalker::enterImportStatement(
    ToolmanParser::ImportStatementContext *node) {
  auto str_lit = node->getToken(ToolmanLexer::StringLiteral, 0)->getText();
  import_builder_.start_import(str_lit.substr(1, str_lit.length() - 2));
}

void DeclPhaseWalker::exitImportStatement(
    ToolmanParser::ImportStatementContext *node) {
  import_builder_.end_import();

  // import regular imports.
  for (auto const &[filepath, import_names] : import().get_regular_imports()) {
    std::shared_ptr<Module> module;
    try {
      module = compiler()->compile_module(filepath);
    } catch (FileNotFoundError &e) {
      push_error(UnresolvedImportError(filepath.filename().string()));
      continue;
    }

    for (auto const &import_name : import_names) {
      if (auto import_type =
              module->type_scope()->lookup(import_name.original_name);
          import_type.has_value()) {
        if (import_name.local_name.has_value()) {
          type_scope_->declare(import_type.value(),
                               import_name.local_name.value());
        } else {
          type_scope_->declare(import_type.value());
        }
      } else {
        push_error(ImportError(import_name.original_name, filepath));
      }
    }
  }

  // import namespace.
  for (auto const &filepath : import().get_namespaces_imports()) {
    std::shared_ptr<Module> module;
    try {
      module = compiler()->compile_module(filepath);
    } catch (FileNotFoundError &e) {
      push_error(UnresolvedImportError(filepath.filename().string()));
      continue;
    }
    for (auto it = module->type_scope()->cbegin();
         it != module->type_scope()->cend(); it++) {
      type_scope_->declare(it->second, it->first);
    }
    //    for (auto const&[name, type] : *module->type_scope()) {
    //        type_scope_->declare(type, name);
    //    }
  }
}

void DeclPhaseWalker::enterFromImport(ToolmanParser::FromImportContext *node) {
  import_builder_.set_import_star(false);
}

void DeclPhaseWalker::enterFromImportStar(
    ToolmanParser::FromImportStarContext *) {
  import_builder_.set_import_star(true);
}

void DeclPhaseWalker::enterImportName(ToolmanParser::ImportNameContext *node) {
  import_builder_.start_import_name(node->identifierName()->getText());
}

void DeclPhaseWalker::enterImportNameAlias(
    ToolmanParser::ImportNameAliasContext *node) {
  import_builder_.start_import_name_alias(node->identifierName()->getText());
}

void FieldTypeBuilder::start_type(const std::shared_ptr<Type> &type) {
  if (!type_stack_.empty()) {
    if (type_stack_.top()->is_list()) {
      if (TypeLocation::ListElement == current_type_location_) {
        auto list_type = std::dynamic_pointer_cast<ListType>(type_stack_.top());
        list_type->set_elem_type(type);
      }

    } else if (type_stack_.top()->is_map()) {
      auto map_type = std::dynamic_pointer_cast<MapType>(type_stack_.top());

      if (TypeLocation::MapKey == current_type_location_) {
        // The key of the map must be a primitive type.
        if (!type->is_primitive()) {
          throw MapKeyTypeMustBePrimitiveError(type);
        } else {
          map_type->set_key_type(
              std::dynamic_pointer_cast<PrimitiveType>(type));
        }
      } else if (TypeLocation::MapValue == current_type_location_) {
        map_type->set_value_type(type);
      }
    }
  }

  if (type->is_map() || type->is_list()) {
    type_stack_.push(type);
  } else {
    current_single_type_ = type;
  }
}

std::shared_ptr<Type> FieldTypeBuilder::end_map_or_list_type() {
  auto top = type_stack_.top();
  type_stack_.pop();
  if (type_stack_.empty()) {
    return top;
  }
  return std::shared_ptr<Type>(nullptr);
}

std::shared_ptr<Type> FieldTypeBuilder::end_single_type() {
  if (type_stack_.empty()) {
    return current_single_type_;
  }
  return std::shared_ptr<Type>(nullptr);
}

}  // namespace toolman
