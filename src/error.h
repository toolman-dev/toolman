// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_ERROR_H_
#define TOOLMAN_ERROR_H_

#include <algorithm>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "src/enum_field.h"
#include "src/option.h"
#include "src/stmt_info.h"
#include "src/type.h"

namespace toolman {

class Error : public std::exception {
 public:
  enum class ErrorType : char { Lexer, Syntax, Semantic };
  enum class Level : char { Note, Warning, Fatal };

  template <typename S>
  Error(ErrorType type, Level level, S&& message)
      : type_(type), level_(level), message_(std::forward<S>(message)) {}

  [[nodiscard]] bool is_fatal() const { return level_ == Level::Fatal; }

  [[nodiscard]] virtual std::string error() const { return message_; }

  [[nodiscard]] const char* what() const noexcept override {
    return error().c_str();
  }

 protected:
  ErrorType type_;
  Level level_;
  std::string message_;
};

class HasMultiError {
 public:
  HasMultiError() = default;
  explicit HasMultiError(std::vector<Error> errors)
      : errors_(std::move(errors)) {}

  [[nodiscard]] bool has_error() const { return !errors_.empty(); }

  [[nodiscard]] bool has_fatal_error() const {
    return std::any_of(errors_.cbegin(), errors_.cend(),
                       [](auto err) { return err.is_fatal(); });
  }

  std::vector<Error> get_errors() { return errors_; }

  void push_error(Error error) { errors_.push_back(std::move(error)); }

 private:
  std::vector<Error> errors_;
};

class DuplicateTypeDeclError final : public Error {
 public:
  template <typename SI>
  DuplicateTypeDeclError(std::shared_ptr<Type> first_declared_type,
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

class DuplicateFieldDeclError final : public Error {
 public:
  template <typename FIELD, typename SI>
  DuplicateFieldDeclError(FIELD first_decl_field, SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "field `" + first_decl_field.get_name() +
                  "` is already declared") {}
};

class DuplicateEnumFieldValueError final : public Error {
 public:
  template <typename SI>
  DuplicateEnumFieldValueError(const EnumField& first_value_field,
                               SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "discriminant value `" +
                  std::to_string(first_value_field.get_value()) +
                  "` already exists") {}
};

class RecursiveOneofTypeError final : public Error {
 public:
  template <typename SI>
  explicit RecursiveOneofTypeError(SI&& stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "oneof type does not allow recursion") {}
};

class UnknownOptionError final : public Error {
 public:
  template <typename SI>
  explicit UnknownOptionError(const std::string& option_name,
                              SI&& option_name_stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "Option \"" + option_name + "\" unknown.") {}
};

class OptionTypeMismatchError final : public Error {
 public:
  template <typename SI>
  OptionTypeMismatchError(Option* option, SI&& option_value_stmt_info)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "Value must be " + option->type_name() + " for " +
                  option->type_name() + " option \"" + option->get_name() +
                  "\".") {}
};

class UnresolvedImportError final : public Error {
 public:
  explicit UnresolvedImportError(const std::string& origin_name)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "ModuleNotFoundError: unresolved import `" + origin_name + "`") {}
};

class ImportError final : public Error {
 public:
  ImportError(const std::string& origin_name, const std::string& filename)
      : Error(Error::ErrorType::Semantic, Error::Level::Fatal,
              "ImportError: cannot import name `" + origin_name + "` from `" +
                  filename + "`") {}
};

// normal exception
class FileNotFoundError : std::exception {
 public:
  explicit FileNotFoundError(std::shared_ptr<std::filesystem::path> filepath)
      : filepath_(std::move(filepath)) {}

  std::shared_ptr<std::filesystem::path> filepath() { return filepath_; }

 private:
  std::shared_ptr<std::filesystem::path> filepath_;
};

}  // namespace toolman

#endif  // TOOLMAN_ERROR_H_
