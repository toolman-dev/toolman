// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_OPTION_H_
#define TOOLMAN_OPTION_H_

#include <string>

namespace toolman {

class Option {
 public:
  explicit Option(std::string name) : name_(std::move(name)) {}

  [[nodiscard]] virtual bool is_bool() const { return false; }
  [[nodiscard]] virtual bool is_numeric() const { return false; }
  [[nodiscard]] virtual bool is_string() const { return false; }

  [[nodiscard]] virtual std::string type_name() const = 0;

  [[nodiscard]] const std::string& get_name() const { return name_; }

 private:
  std::string name_;
};

class BoolOption final : public Option {
 public:
  using Option::Option;

  [[nodiscard]] bool is_bool() const override { return true; }
  [[nodiscard]] bool get_value() const { return value_; }
  void set_value(bool value) { value_ = value; }

  [[nodiscard]] std::string type_name() const override { return "bool"; }

 private:
  bool value_;
};

class NumericOption final : public Option {
 public:
  using Option::Option;

  [[nodiscard]] bool is_numeric() const override { return true; }

  [[nodiscard]] double get_value() const { return value_; }
  void set_value(double value) { value_ = value; }
  [[nodiscard]] std::string type_name() const override { return "numeric"; }

 private:
  double value_;
};

class StringOption final : public Option {
 public:
  using Option::Option;

  [[nodiscard]] bool is_string() const override { return true; }
  [[nodiscard]] const std::string& get_value() const { return value_; }
  void set_value(std::string value) { value_ = std::move(value); }
  [[nodiscard]] std::string type_name() const override { return "string"; }

 private:
  std::string value_;
};

}  // namespace toolman

#endif  // TOOLMAN_OPTION_H_
