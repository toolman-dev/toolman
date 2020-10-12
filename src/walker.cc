// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "src/walker.h"

namespace toolman {

void FieldTypeBuilder::start_type(const std::shared_ptr<Type>& type) {
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
