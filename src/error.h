// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ERROR_H_
#define TOOLMAN_ERROR_H_

#include <string>
#include <utility>

#include "src/stmt_info.h"

namespace toolman {
class Error {
 public:
  enum class Type : char { Lexer, Syntax, Semantic };
  enum class Level : char { Note, Warning, Fatal };

  template <typename S, typename SI>
  Error(Type type, Level level, S&& message, SI&& stmt_info)
      : type_(type),
        level_(level),
        message_(std::forward<S>(message)),
        stmt_info_(std::forward<SI>(stmt_info)) {}

  [[nodiscard]] virtual std::string error() const { return message_; };

 protected:
  Type type_;
  Level level_;
  StmtInfo stmt_info_;
  std::string message_;
};

class DuplicateDeclError final : public Error {
 public:
  template <typename S, typename SI>
  DuplicateDeclError(S&& message, SI&& stmt_info)
      : Error(Error::Type::Semantic, Error::Level::Fatal,
              std::forward<S>(message), std::forward<SI>(stmt_info)) {}
};

class LiteralElementTypeMismatchError final : public Error {
 public:
  template <typename S, typename SI>
  LiteralElementTypeMismatchError(S&& message, SI&& stmt_info)
      : Error(Error::Type::Semantic, Error::Level::Fatal,
              std::forward<S>(message), std::forward<SI>(stmt_info)) {}
};

class FieldTypeMismatchError final : public Error {
 public:
  template <typename S, typename SI1, typename SI2>
  FieldTypeMismatchError(S&& message, SI1&& field_stmt_info,
                         SI2&& literal_stmt_info)
      : Error(Error::Type::Semantic, Error::Level::Fatal,
              std::forward<S>(message), std::forward<SI1>(field_stmt_info)),
        literal_stmt_info_(std::forward<SI2>(literal_stmt_info)) {}

 private:
  StmtInfo literal_stmt_info_;
};

}  // namespace toolman

#endif  // TOOLMAN_ERROR_H_
