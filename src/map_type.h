// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_MAP_TYPE_H_
#define TOOLMAN_MAP_TYPE_H_

#include <string>
#include <utility>
#include <memory>

#include "src/type.h"
#include "src/primitive_type.h"

namespace toolman {

class MapType final : public Type {
 public:
    using KeyType = PrimitiveType;
    using ValueType = Type;

    MapType(const std::string &name,
        std::shared_ptr <PrimitiveType> key_type,
        std::shared_ptr <Type> value_type,
        unsigned int line_no,
        unsigned int column_no,
        std::string file)
        : Type(name, line_no, column_no, std::move(file)),
        key_type_(std::move(key_type)),
            value_type_(std::move(value_type)) {}

    [[nodiscard]] bool is_map() const override { return true; }

    [[nodiscard]] const std::shared_ptr <PrimitiveType> &get_key_type() const {
        return key_type_;
    }

    [[nodiscard]] const std::shared_ptr <Type> &get_value_type() const
        { return value_type_; }

    [[nodiscard]] std::string to_string() const override {
        return "{" + key_type_->to_string()
            + ", " + value_type_->to_string() +"}";
    }

 private:
    // In toolman, map key must be primitive type.
    std::shared_ptr <KeyType> key_type_;
    std::shared_ptr <ValueType> value_type_;
};

}  // namespace toolman
#endif  // TOOLMAN_MAP_TYPE_H_
