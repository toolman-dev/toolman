// Copyright 2020 the Toolman project authors. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#ifndef TOOLMAN_GOLANG_GENERATOR_H_
#define TOOLMAN_GOLANG_GENERATOR_H_

#include <memory>
#include <sstream>
#include <string>

#include "src/generator.h"
#include "src/list_type.h"
#include "src/map_type.h"
#include "src/primitive_type.h"

namespace toolman {
class GolangGenerator : public Generator {
 public:
  void generate_struct(const StructType& struct_type) override {
    std::stringstream code;
    code << "type " << struct_type.get_name() << "struct {" << std::endl;

    for (const auto& field : struct_type.get_fields()) {
      code << capitalize(field.get_name()) << " "
           << type_to_go_type(field.get_type())
           << " `json:\"" + field.get_name() + "\"`" << std::endl;
    }
    code << "}";
  }

  void generate_enum(const EnumType& enum_type) override {
    std::stringstream code;

    auto capitalized_name = capitalize(enum_type.get_name());
    code << "type " << capitalized_name << "int32" << std::endl;
    code << "const (" << std::endl;
    for (const auto& field : enum_type.get_fields()) {
      code << capitalized_name << "_" << field.get_name() << " "
           << capitalized_name << " = " << field.get_value() << std::endl;
    }
    code << ")" << std::endl;
  }

 private:
  std::string capitalize(std::string in) const {
    in[0] = toupper(in[0]);
    return in;
  }

  std::string decapitalize(std::string in) const {
    in[0] = tolower(in[0]);
    return in;
  }

  std::string type_to_go_type(const std::shared_ptr<Type> type) const {
    if (type->is_primitive()) {
      auto primitive = std::dynamic_pointer_cast<PrimitiveType>(type);
      if (primitive->is_bool()) {
        return "bool";
      } else if (primitive->is_i32()) {
        return "int32";
      } else if (primitive->is_u32()) {
        return "uint32";
      } else if (primitive->is_i64()) {
        return "int64";
      } else if (primitive->is_u64()) {
        return "uint64";
      } else if (primitive->is_float()) {
        return "float64";
      } else if (primitive->is_string()) {
        return "string";
      } else if (primitive->is_any()) {
        return "interface{}";
      }
    } else if (type->is_struct() || type->is_enum()) {
      return capitalize(type->get_name());
    } else if (type->is_list()) {
      auto list = std::dynamic_pointer_cast<ListType>(type);
      return "[]" + type_to_go_type(list->get_elem_type());
    } else if (type->is_map()) {
      auto map = std::dynamic_pointer_cast<MapType>(type);
      return "map[" + type_to_go_type(map->get_key_type()) + "]" +
             type_to_go_type(map->get_value_type());
    }
  }
};
}  // namespace toolman

#endif  // TOOLMAN_GOLANG_GENERATOR_H_
