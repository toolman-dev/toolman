// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_LITERAL_H_
#define TOOLMAN_LITERAL_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "src/list_type.h"
#include "src/map_type.h"
#include "src/stmt_info.h"
#include "src/type.h"

namespace toolman {

class Literal : public HasStmtInfo {
 public:
  template <typename SI>
  explicit Literal(SI&& stmt_info) : HasStmtInfo(std::forward<SI>(stmt_info)) {}
  [[nodiscard]] virtual bool is_primitive() const { return false; }
  [[nodiscard]] virtual bool is_list() const { return false; }
  [[nodiscard]] virtual bool is_map() const { return false; }
  virtual std::string to_string() const = 0;
  virtual ~Literal() = default;
};

class PrimitiveLiteral final : public Literal {
 public:
  template <typename SI, typename S>
  PrimitiveLiteral(S&& value, SI&& stmt_info)
      : Literal(std::forward<SI>(stmt_info)), value_(std::forward<>(value)) {}

  [[nodiscard]] const std::string& get_value() const { return value_; }
  [[nodiscard]] bool is_primitive() const override { return true; }

  std::string to_string() const override { return value_; }

 private:
  std::string value_;
};

class MapLiteral final : public Literal {
 public:
  template <typename SI>
  MapLiteral(SI&& stmt_info) : HasStmtInfo(std::forward<SI>(stmt_info)) {}

  // Insert a k-v literal pair into the map literal.
  // If the key or value type did not match
  // the method will throw TypeError exception.
  template <typename P>
  void insert(P&& kv_pair);

  [[nodiscard]] const std::map<PrimitiveLiteral, std::unique_ptr<Literal>>&
  get_value() const {
    return value_;
  }

  [[nodiscard]] bool is_map() const override { return true; }

  std::string to_string() const override { return "map"; }

 private:
  std::map<PrimitiveLiteral, std::unique_ptr<Literal>> value_;
};

class ListLiteral final : public Literal {
 public:
  template <typename SI>
  ListLiteral(SI&& stmt_info) : Literal(std::forward<SI>(stmt_info)) {}

  // Insert a value literal into the list literal.
  // If the value type did not match
  // the method will throw TypeError exception.
  template <typename L>
  void insert(L&& literal_value);

  [[nodiscard]] const std::vector<std::unique_ptr<Literal>>& get_value() const {
    return value_;
  }
  [[nodiscard]] bool is_list() const override { return true; }
  std::string to_string() const override { return "list"; }

 private:
  std::vector<std::unique_ptr<Literal>> value_;
};

}  // namespace toolman

#endif  // TOOLMAN_LITERAL_H_