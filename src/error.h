// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ERROR_H_
#define TOOLMAN_ERROR_H_

#include <memory>
#include <string>
#include <utility>

#include "src/stmt_info.h"
#include "src/type.h"

namespace toolman {
class Error {
 public:
  enum class ErrorType : char { Lexer, Syntax, Semantic };
  enum class Level : char { Note, Warning, Fatal };

  template <typename S>
  Error(ErrorType type, Level level, S&& message)
      : type_(type), level_(level), message_(std::forward<S>(message)) {}

  [[nodiscard]] virtual std::string error() const { return message_; }

 protected:
  ErrorType type_;
  Level level_;
  std::string message_;
};

class TypeDuplicateDeclError final : public Error {
 public:
  template <typename SI>
  TypeDuplicateDeclError(std::shared_ptr<Type> first_declared_type,
                         SI&& duplicate_decl_stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "A type " + first_declared_type->to_string() +
                  " has been defined more than once."),
        first_declared_type_(std::move(first_declared_type)),
        duplicate_decl_stmt_info_(std::forward<SI>(duplicate_decl_stmt_info)) {}

 private:
  std::shared_ptr<Type> first_declared_type_;
  StmtInfo duplicate_decl_stmt_info_;
};

class MapKeyTypeMustBePrimitiveError final : public Error {
 public:
  explicit MapKeyTypeMustBePrimitiveError(const std::shared_ptr<Type>& key_type)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "The key of the map must be a primitive type. give " +
                  key_type->to_string()),
        key_type_(key_type) {}

 private:
  std::shared_ptr<Type> key_type_;
};

class CustomTypeNotFoundError final : public Error {
 public:
  template <typename SI>
  CustomTypeNotFoundError(const std::string& type_name, SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "cannot find type `" + type_name + "`") {}
};

class FieldDuplicateDeclError final : public Error {
 public:
  template <typename FIELD, typename SI>
  FieldDuplicateDeclError(FIELD first_decl_field, SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "field `" + first_decl_field.get_name() +
                  "` is already declared") {}
};

}  // namespace toolman

#endif  // TOOLMAN_ERROR_H_
