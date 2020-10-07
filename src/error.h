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

  Error(ErrorType type, Level level) : type_(type), level_(level) {}

  template <typename S>
  Error(ErrorType type, Level level, S&& message)
      : type_(type), level_(level), message_(std::forward<S>(message)) {}

  [[nodiscard]] virtual std::string error() const { return message_; }

 protected:
  ErrorType type_;
  Level level_;
  std::string message_;
};

class DuplicateDeclError final : public Error {
 public:
  template <typename SI>
  DuplicateDeclError(std::shared_ptr<Type> first_declared_type,
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

class LiteralElementTypeMismatchError final : public Error {
 public:
  template <typename S, typename SI>
  LiteralElementTypeMismatchError(S&& message, SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              std::forward<S>(message), std::forward<SI>(stmt_info)) {}
};

class FieldTypeMismatchError final : public Error {
 public:
  template <typename S, typename SI1, typename SI2>
  FieldTypeMismatchError(S&& message, SI1&& field_stmt_info,
                         SI2&& literal_stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              std::forward<S>(message), std::forward<SI1>(field_stmt_info)),
        literal_stmt_info_(std::forward<SI2>(literal_stmt_info)) {}

 private:
  StmtInfo literal_stmt_info_;
};

class MapKeyTypeMustBePrimitiveError final : public Error {
 public:
  explicit MapKeyTypeMustBePrimitiveError(std::shared_ptr<Type> key_type)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "The key of the map must be a primitive type. give " +
                  key_type->to_string()) {}
};

class CustomTypeNotFoundError final : public Error {
 public:
  template <typename SI>
  CustomTypeNotFoundError(const std::string& type_name, SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "cannot find type `" + type_name + "`") {}
};

}  // namespace toolman

#endif  // TOOLMAN_ERROR_H_
