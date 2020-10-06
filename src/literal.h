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

#include "src/stmt_info.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/type.h"

namespace toolman {

template <typename V>
class PrimitiveLiteral final : public HasStmtInfo {
 public:
  template <typename SI>
  PrimitiveLiteral(std::shared_ptr<PrimitiveType> type, V&& value,
                   SI&& stmt_info)
      : HasStmtInfo(std::forward<SI>(stmt_info)),
        type_(std::move(type)),
        value_(std::forward<V>(value)) {}

  [[nodiscard]] std::shared_ptr<PrimitiveType> get_type() const {
    return type_;
  }

  const V& get_value() const { return value_; }

 private:
  std::shared_ptr<PrimitiveType> type_;
  V value_;
};

template <typename KL, typename VL>
class MapLiteral final : public HasStmtInfo {
 public:
  template <typename SI>
  MapLiteral(std::shared_ptr<MapType> type, SI&& stmt_info)
      : HasStmtInfo(std::forward<SI>(stmt_info)), type_(std::move(type)) {}

  // Insert a k-v literal pair into the map literal.
  // If the key or value type did not match
  // the method will throw TypeError exception.
  void insert(std::pair<KL, VL> kv_pair);

  [[nodiscard]] std::shared_ptr<MapType> get_type() const { return type_; }
  [[nodiscard]] std::map<KL, VL> get_value() const { return value_; }

 private:
  std::shared_ptr<MapType> type_;
  std::map<KL, VL> value_;
};

template <typename VL>
class ListLiteral final : public HasStmtInfo {
 public:
  template <typename SI>
  ListLiteral(std::shared_ptr<ListType> type, SI&& stmt_info)
      : HasStmtInfo(std::forward<SI>(stmt_info)), type_(std::move(type)) {}

  // Insert a value literal into the list literal.
  // If the value type did not match
  // the method will throw TypeError exception.
  void insert(VL&& value);

  [[nodiscard]] std::shared_ptr<ListType> get_type() const { return type_; }
  [[nodiscard]] std::vector<VL> get_value() const { return value_; }

 private:
  std::shared_ptr<ListType> type_;
  std::vector<VL> value_;
};

}  // namespace toolman

#endif  // TOOLMAN_LITERAL_H_