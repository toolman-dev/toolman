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
  void before_generate_struct(std::ostream& ostream) const override {
    ostream << "type (" << NL;
  }

  void after_generate_struct(std::ostream& ostream) const override {
    ostream << ")" << NL << NL;
  }

  void after_generate_enum(std::ostream& ostream) const override {
    ostream << NL;
  }

  void generate_struct(
      std::ostream& ostream,
      const std::shared_ptr<StructType>& struct_type) const override {
    auto capitalized_struct_name = capitalize(struct_type->get_name();
    for (const auto& field : struct_type->get_fields()) {
      if (field.get_type()->is_oneof()) {
        auto oneof_name =
            gen_oneof_name(struct_type->get_name(), field.get_name());
        ostream << oneof_name + " interface {" << NL << oneof_name << "()" << NL
                << "}" << NL;
        auto oneof = std::dynamic_pointer_cast<OneofType>(field.get_type());
        for (const auto& oneof_field : oneof->get_fields()) {
          auto capitalized_field_name = capitalize(oneof_field.get_name());
          ostream << capitalized_struct_name + "_" + capitalized_field_name
                  << " struct {" << NL << capitalized_field_name << " "
                  << type_to_go_type(oneof_field.get_type()) << NL << "}" << NL;
        }
      }
    }

    ostream << (struct_type->is_public() ? capitalized_struct_name
                                         : struct_type->get_name())
            << " struct {" << NL;
    for (const auto& field : struct_type->get_fields()) {
      ostream << "  " << capitalize(field.get_name()) << " "
              << (field.is_optional() && !field.get_type()->is_map() &&
                          !field.get_type()->is_list()
                      ? "*"
                      : "")
              << type_to_go_type(field.get_type())
              << " `json:\"" + field.get_name() + "\"`" << NL;
    }
    ostream << "}" << NL;
  }

  void generate_enum(
      std::ostream& ostream,
      const std::shared_ptr<EnumType>& enum_type) const override {
    auto capitalized_name = capitalize(enum_type->get_name());
    ostream << "type "
            << (enum_type->is_public() ? capitalized_name
                                       : enum_type->get_name())
            << " int32" << NL;
    ostream << "const (" << NL;
    for (const auto& field : enum_type->get_fields()) {
      ostream << capitalized_name << "_" << field.get_name() << " "
              << capitalized_name << " = " << field.get_value() << NL;
    }
    ostream << ")" << NL;
  }

 private:
  [[nodiscard]] static std::string capitalize(std::string in) {
    in[0] = toupper(in[0]);
    return in;
  }

  [[nodiscard]] static std::string decapitalize(std::string in) {
    in[0] = tolower(in[0]);
    return in;
  }

  [[nodiscaed]] static std::string gen_oneof_name(
      const std::string struct_name, const std::string field_name) {
    return "is" + capitalize(struct_name) + "_" + capitalize(field_name);
  }

  [[nodiscard]] static std::string type_to_go_type(
      const std::shared_ptr<Type>& type) {
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
    return "";
  }
};
}  // namespace toolman

#endif  // TOOLMAN_GOLANG_GENERATOR_H_
